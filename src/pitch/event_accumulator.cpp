#include <algorithm>

#include "pitch_message.h"
#include "event_accumulator.h"

namespace pitchstream
{
    using p_message = event_accumulator::p_message;
    using v_summary = event_accumulator::v_summary;
    using v_summary_item = event_accumulator::v_summary_item;

    static auto summary_comp = [] (const v_summary_item & v1, const v_summary_item & v2) {
        return v1.second > v2.second;
    };

    void event_accumulator::process_message(p_message &&input)
    {
        if (!input)
            return; // ignore null pointer

        switch (input->get_type())
        {
        case message_type::add_order:
            return process_add(move(input));
        case message_type::order_executed:
            return process_executed(move(input));
        case message_type::order_cancel:
            return process_cancel(move(input));
        case message_type::trade_message:
            return process_trade(move(input));
        }
    }

    v_summary event_accumulator::generate_summary_all()
    {
        event_accumulator::v_summary res;
        res.reserve(counters.size());
        std::copy(counters.begin(), counters.end(), back_inserter(res));
        std::sort(res.begin(), res.end(), summary_comp);
        return res;
    }

    v_summary event_accumulator::generate_summary_n(int n)
    {
        if (n > counters.size())
            return generate_summary_all();
        event_accumulator::v_summary res(n);
        std::partial_sort_copy(counters.begin(), counters.end(),
                        res.begin(), res.end(), summary_comp);

        return res;
    }

    void event_accumulator::add(event_accumulator & other) {
        for (auto & p : other.counters) {
            counters[p.first] += p.second;
        }
    }

    void event_accumulator::process_add(p_message &&input)
    {
        live_orders[input->get_order_id()] = move(input);
    }
    
    void event_accumulator::process_cancel(p_message &&input)
    {
        auto it = live_orders.find(input->get_order_id());
        if (it == live_orders.end())
            throw std::out_of_range("canceling order that does not exist");
        if (it->second->get_shares_count() < input->get_shares_count())
        {
            throw std::out_of_range("canceling more shares than in live order");
        }
        if (!it->second->subtract_shares_count(input->get_shares_count()))
        {
            live_orders.erase(input->get_order_id());
        }
    }

    void event_accumulator::process_executed(p_message &&input)
    {
        uint32_t num_exec = input->get_shares_count();
        auto it = live_orders.find(input->get_order_id());
        if (it == live_orders.end())
            throw std::out_of_range("executing order that does not exist");
        if (it->second->get_shares_count() < num_exec)
        {
            throw std::out_of_range("executing more shares than in live order");
        }
        counters[it->second->get_stock_symbol()] += num_exec;
        if (!it->second->subtract_shares_count(num_exec))
        {
            live_orders.erase(input->get_order_id());
        }
    }
    
    void event_accumulator::process_trade(p_message &&input)
    {        
        
        counters[input->get_stock_symbol()] += input->get_shares_count();

    }
}
