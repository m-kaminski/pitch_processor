#ifndef _EVENT_ACCUMULATOR_H
#define _EVENT_ACCUMULATOR_H
#include <memory>
#include <vector>
#include <unordered_map>
#include <utility>

#include "pitch_message.h"
namespace pitchstream
{

    class event_accumulator
    {
        public:
        using p_message = std::unique_ptr<pitch_message>;
        using v_summary = std::vector<std::pair<uint64_t, std::string>>;

        void process_message(p_message && input);

        v_summary generate_summary_all();
        v_summary generate_summary_n(int n);

        private:

        // live orders, unless cancelled/filled
        std::unordered_map<uint64_t, p_message> live_orders;

        // count of trades executed per symbol
        std::unordered_map<std::string, uint64_t> counters;
    };
}
#endif