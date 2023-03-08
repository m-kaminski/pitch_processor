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
    using v_summary_item = event_accumulator::v_summary_item;
    using v_summary = event_accumulator::v_summary;
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
        EXPECT_EQ(a.generate_summary_all()[0], v_summary_item("AAPL", 40));
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
        EXPECT_EQ(a.generate_summary_all()[0], v_summary_item("AAPL", 100));
    }

    // here go scenarios related to sorting
    TEST_F(event_accumulator_test, multiple_trades_summary_all)
    {
        event_accumulator a;
        a.process_message(move(p_message(generate_add(1, 100, "BA"))));
        a.process_message(move(p_message(generate_execute(1, 40))));
        a.process_message(move(p_message(generate_execute(1, 60))));
        a.process_message(move(p_message(generate_trade(1, 30, "BA"))));
        a.process_message(move(p_message(generate_trade(1, 300, "NG"))));
        a.process_message(move(p_message(generate_trade(1, 900, "LMT"))));

        v_summary summary = a.generate_summary_all();
        EXPECT_FALSE(summary.empty());
        EXPECT_EQ(summary.size(), 3);
        EXPECT_EQ(summary[0], v_summary_item("LMT", 900));
        EXPECT_EQ(summary[1], v_summary_item("NG", 300));
        EXPECT_EQ(summary[2], v_summary_item("BA", 130));
    }

    TEST_F(event_accumulator_test, multiple_trades_summary_n)
    {
        event_accumulator a;
        a.process_message(move(p_message(generate_trade(1, 200, "BA"))));
        a.process_message(move(p_message(generate_trade(1, 100, "NG"))));
        a.process_message(move(p_message(generate_trade(1, 90, "LMT"))));

        v_summary summary = a.generate_summary_n(2);
        EXPECT_EQ(summary.size(), 2);
        EXPECT_EQ(summary[0], v_summary_item("BA", 200));
        EXPECT_EQ(summary[1], v_summary_item("NG", 100));
    }

    TEST_F(event_accumulator_test, accumulator_addition)
    {
        event_accumulator a1;
        event_accumulator a2;
        a1.process_message(move(p_message(generate_trade(1, 200, "BA"))));
        a1.process_message(move(p_message(generate_trade(1, 100, "NG"))));
        a2.process_message(move(p_message(generate_trade(1, 400, "BA"))));
        a2.process_message(move(p_message(generate_trade(1, 300, "LMT"))));

        a1.add(a2);
        v_summary summary = a1.generate_summary_n(2);
        EXPECT_EQ(summary.size(), 2);
        EXPECT_EQ(summary[0], v_summary_item("BA", 600));
        EXPECT_EQ(summary[1], v_summary_item("LMT", 300));
    }

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
