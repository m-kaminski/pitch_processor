#include "gtest/gtest.h"

#include "pitch_message.h"
#include "event_accumulator.h"
#include <string>
#include <memory>
#include <cstdint>
#include <utility>
#include <unordered_map>

namespace pitchstream
{
    pitch_message *generate_add(uint64_t order_id, uint32_t shares_count, const std::string &stock_symbol);
    pitch_message *generate_execute(uint64_t order_id, uint32_t shares_count);
    pitch_message *generate_cancel(uint64_t order_id, uint32_t shares_count);
    pitch_message *generate_trade(uint64_t order_id, uint32_t shares_count, const std::string &stock_symbol);

    class event_accumulator_test : public ::testing::Test
    {
    };
    using p_message = std::unique_ptr<pitch_message>;
    using summary_res = std::pair<std::string, uint64_t>;
    TEST_F(event_accumulator_test, null_pointer_ignored)
    {
        event_accumulator a;
        p_message p_m;
        a.process_message(move(p_m));
        EXPECT_TRUE(a.generate_summary_all().empty());
    }

    TEST_F(event_accumulator_test, inserting_empty)
    {
        event_accumulator a;
        p_message p_m(generate_add(1, 0, "AAPL"));
        a.process_message(move(p_m));
        EXPECT_TRUE(a.generate_summary_all().empty());
    }

    // simple execute-related scenarios
    TEST_F(event_accumulator_test, inserting_add_and_execute)
    {
        event_accumulator a;
        a.process_message(move(p_message(generate_add(1, 100, "AAPL"))));
        a.process_message(move(p_message(generate_execute(1, 40))));
        EXPECT_FALSE(a.generate_summary_all().empty());
        EXPECT_EQ(a.generate_summary_all()[0], summary_res("AAPL", 40));
    }

    TEST_F(event_accumulator_test, full_execution_erases_open)
    {

        event_accumulator a;
        a.process_message(move(p_message(generate_add(1, 100, "AAPL"))));
        a.process_message(move(p_message(generate_execute(1, 40))));
        EXPECT_EQ(a.live_orders.size(), 1);
        a.process_message(move(p_message(generate_execute(1, 60))));
        EXPECT_EQ(a.live_orders.size(), 0);
    }

    // simple cancel-related scenarios
    TEST_F(event_accumulator_test, inserting_add_and_cancel)
    {
        event_accumulator a;
        a.process_message(move(p_message(generate_add(1, 100, "AAPL"))));
        a.process_message(move(p_message(generate_cancel(1, 40))));
        EXPECT_TRUE(a.generate_summary_all().empty());
    }

    TEST_F(event_accumulator_test, full_cancel_erases_open)
    {

        event_accumulator a;
        a.process_message(move(p_message(generate_add(1, 100, "AAPL"))));
        a.process_message(move(p_message(generate_cancel(1, 40))));
        EXPECT_EQ(a.live_orders.size(), 1);
        a.process_message(move(p_message(generate_cancel(1, 60))));
        EXPECT_EQ(a.live_orders.size(), 0);
    }

    // simple trade related scenarios
    TEST_F(event_accumulator_test, single_trade)
    {
        event_accumulator a;
        a.process_message(move(p_message(generate_trade(1, 100, "AAPL"))));
        EXPECT_FALSE(a.generate_summary_all().empty());
        EXPECT_EQ(a.generate_summary_all()[0], summary_res("AAPL", 100));
    }

    // here go scenarios related to sorting


    // helper functions
    pitch_message *generate_add(uint64_t order_id, uint32_t shares_count, const std::string &stock_symbol)
    {
        pitch_message *p = new pitch_message(message_type::add_order);
        p->shares_count = shares_count;
        p->order_id = order_id;
        p->stock_symbol = stock_symbol;
        return p;
    }

    pitch_message *generate_execute(uint64_t order_id, uint32_t shares_count)
    {
        pitch_message *p = new pitch_message(message_type::order_executed);
        p->shares_count = shares_count;
        p->order_id = order_id;
        return p;
    }

    pitch_message *generate_cancel(uint64_t order_id, uint32_t shares_count)
    {
        pitch_message *p = new pitch_message(message_type::order_cancel);
        p->shares_count = shares_count;
        p->order_id = order_id;
        return p;
    }
    pitch_message *generate_trade(uint64_t order_id, uint32_t shares_count, const std::string &stock_symbol)
    {
        pitch_message *p = new pitch_message(message_type::trade_message);
        p->shares_count = shares_count;
        p->order_id = order_id;
        p->stock_symbol = stock_symbol;
        return p;
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
