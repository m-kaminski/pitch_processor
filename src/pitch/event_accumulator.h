#ifndef _EVENT_ACCUMULATOR_H
#define _EVENT_ACCUMULATOR_H
#include <memory>
#include <vector>
#include <unordered_map>
#include <utility>
#include <gtest/gtest_prod.h>

#include "pitch_message.h"
namespace pitchstream
{

    class event_accumulator
    {
    public:
        using v_summary_item = std::pair<std::string, uint64_t>;
        using p_message = std::unique_ptr<pitch_message>;
        using v_summary = std::vector<v_summary_item>;

        void process_message(p_message &&input);

        v_summary generate_summary_all();
        v_summary generate_summary_n(int n);
    void add(event_accumulator & other);

    private:
        // live orders, unless cancelled/filled
        std::unordered_map<uint64_t, p_message> live_orders;

        // count of trades executed per symbol
        std::unordered_map<std::string, uint64_t> counters;

        void process_add(p_message &&input);
        void process_cancel(p_message &&input);
        void process_executed(p_message &&input);
        void process_trade(p_message &&input);
        FRIEND_TEST(event_accumulator_test, full_cancel_erases_open);
        FRIEND_TEST(event_accumulator_test, full_execution_erases_open);
    };
}
#endif
