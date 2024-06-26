#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#include <cassert>
#include <iomanip>

#include "../pitch/summary_fotmatter.h"
#include "../pitch/pitch_message.h"
#include "../pitch/pitch_decoder.h"
#include "../pitch/event_accumulator.h"
#include "worker_thread.h"
#include "execution_policy_multi_threaded.h"

namespace pitchstream
{
    void execution_policy_multi_threaded::run()
    {
        assert(thread_data.size() == num_threads);
        worker_thread wt;

        int lock_fail = 0;
        int lock_success = 0;

        for (int i = 0; i != num_threads; ++i) {
            thread_data[i].pre_input_length_cap = 1024;
            thread_data[i].pre_input.reserve(thread_data[i].pre_input_length_cap + 100);
        }

        wt.set_run_function([&](worker_thread *w)
                            { this->process_input_stage2(w); });

        wt.run_with_children(num_threads, forced_affinity);

        ioe->process_input([&](const char *B, const char *E)
                           { this->process_input_stage1(B, E); });

        for (int i = 0; i != num_threads; ++i)
        {
            if (thread_data[i].pre_input.size())
            {
                std::unique_lock<std::mutex> lock(thread_data[i].queue_mutex);
                thread_data[i].inputs.emplace_back(move(thread_data[i].pre_input));
            }
            thread_data[i].mutex_condition.notify_one();
        }

        // end of file, send empty to each thread to terminate
        for (int i = 0; i != num_threads; ++i)
        {
            {
                std::unique_lock<std::mutex> lock(thread_data[i].queue_mutex);
                std::string empty("");
                thread_data[i].inputs.push_back(move(empty));
            }
            thread_data[i].mutex_condition.notify_one();
        }

        wt.join_with_children();

        // add up results
        for (int i = 1; i != num_threads; ++i)
        {
            thread_data[0].a.add(thread_data[i].a);
        }
        format_summary(std::cout, thread_data[0].a.generate_summary_n(num_results));

        if (errors.has_errors())
            errors.print_errors();

        if (lock_fail * 1000 > (lock_success + lock_fail)) // if lock fail rate > 0.1%
            std::cerr << "lock failure rate : " << std::fixed << std::setprecision(5)
                      << double(100.0 * lock_fail) / double(lock_fail + lock_success)
                      << "%. Consider increasing thread count to improve performance." 
                      << std::endl;
    }

    void execution_policy_multi_threaded::process_input_stage1(const char *begin, const char *end)
    {
        if (end - begin < COMMON_ORDER_ID_OFFSET + COMMON_ORDER_ID_LENGTH)
        {
            errors.lines_skipped++;
            return; /* string too short, no order ID  - ignore silently */
        }

        // calculate very simple hash based on order ID, just to make sure that any two
        // messages with the same order ID will get routed to the same worker thread,
        // whine maintaining ordering within order ID
        uint32_t * order_id_uint = (uint32_t *)(begin + COMMON_ORDER_ID_OFFSET);
        int thread_id = (order_id_uint[0] + order_id_uint[1] - order_id_uint[2])  %
                        num_threads;

        thread_data[thread_id].pre_input.append(begin, end);
        thread_data[thread_id].pre_input += '\n';

        if (thread_data[thread_id].pre_input.size() > thread_data[thread_id].pre_input_length_cap)
        {
            bool locked = false;
            {
                std::unique_lock<std::mutex> lock(thread_data[thread_id].queue_mutex, std::try_to_lock);
                if (lock.owns_lock())
                {
                    locked = true;
                    thread_data[thread_id].inputs.emplace_back(move(thread_data[thread_id].pre_input));
                }
            }
            if (locked)
            {
                lock_success++;
                thread_data[thread_id].mutex_condition.notify_one();
                if (thread_data[thread_id].pre_input_length_cap < pre_input_max_length)
                    thread_data[thread_id].pre_input_length_cap *= 2;
                thread_data[thread_id].pre_input.reserve(thread_data[thread_id].pre_input_length_cap + 100);
            }
            else
            {
                lock_fail++;
            }
        }
    }

    void execution_policy_multi_threaded::process_input_stage2(worker_thread *w)
    {
        int thread_id = w->get_id();
        while (true)
        {
            bool break_condition = false;
            std::vector<std::string> messages;
            {
                std::unique_lock<std::mutex> lock(thread_data[thread_id].queue_mutex);
                thread_data[thread_id].mutex_condition.wait(lock, [&]
                                                            { return !thread_data[thread_id].inputs.empty(); });
                messages.swap(thread_data[thread_id].inputs);
            }

            for (std::string &message : messages)
            {
                if (message.empty())
                {
                    break_condition = true;
                    break;
                }

                auto begin = message.begin();
                auto middle = begin;
                auto end = message.end();
                while (middle != end)
                {
                    auto new_middle = std::find(middle, end, '\n');

                    pitch_decoder::p_message event;
                    try
                    {
                        event = pitch_decoder::decode(middle, new_middle);
                        if (!event)
                        {
                            errors.lines_skipped++;
                        }
                    }
                    catch (...)
                    {
                        errors.parse_error_counter++;
                    }
                    try
                    {
                        thread_data[thread_id].a.process_message(std::move(event));
                    }
                    catch (...)
                    {
                        errors.processing_error_counter++;
                    }
                    middle = new_middle + 1;
                }
            }
            if (break_condition)
                break;
        }
    }

    void execution_policy_multi_threaded::join_results()
    {
    }

    execution_policy_multi_threaded::thread_status::~thread_status()
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (!inputs.empty())
        {
            std::cerr << "Thread queue is not clean. This may be data corruption" << std::endl;
        }
    }
}