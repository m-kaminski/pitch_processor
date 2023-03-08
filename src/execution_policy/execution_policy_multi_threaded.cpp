#include <iostream>
#include <string>
#include <fstream>
#include <iterator>

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
        worker_thread wt;

        for (int i = 0; i != num_threads; ++i)
            thread_data[i].pre_input.reserve(multistring_length + 100);
        wt.set_run_function([&](worker_thread *w)
                            { this->process_input_stage2(w); });

        wt.run_with_children(num_threads);
        ioe->process_input([&](const char *B, const char *E)
                           { this->process_input_stage1(B, E); });
        // end of file, send empty to each thread to terminate

        for (int i = 0; i != num_threads; ++i)
        {
            {
                std::unique_lock<std::mutex> lock(thread_data[i].queue_mutex);
                thread_data[i].inputs.emplace_back(move(thread_data[i].pre_input));
                thread_data[i].inputs.push_back(std::string());
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
    }



    void execution_policy_multi_threaded::process_input_stage1(const char *begin, const char *end)
    {

        int thread_id =
            std::accumulate(begin + COMMON_ORDER_ID_OFFSET,
                            begin + COMMON_ORDER_ID_OFFSET + COMMON_ORDER_ID_LENGTH,
                            0) %
            num_threads;

        thread_data[thread_id].pre_input.append(begin, end);
        thread_data[thread_id].pre_input += '\n';

        if (thread_data[thread_id].pre_input.size() > multistring_length)
        {
            {
                std::unique_lock<std::mutex> lock(thread_data[thread_id].queue_mutex);

                thread_data[thread_id].inputs.emplace_back(move(thread_data[thread_id].pre_input));
            }
            thread_data[thread_id].mutex_condition.notify_one();
            thread_data[thread_id].pre_input.reserve(multistring_length + 100);
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
                try
                {

                    auto begin = message.begin();
                    auto middle = begin;
                    auto end = message.end();
                    while (middle != end)
                    {
                        auto new_middle = std::find(middle, end, '\n');

                        thread_data[thread_id].a.process_message(std::move(
                            pitch_decoder::decode(middle, new_middle)));
                        middle = new_middle + 1;
                    }
                }
                catch (...)
                {
                    std::cerr << "failing on " << message << std::endl;
                    throw;
                }
            }
            if (break_condition)
                break;
        }
    }

    void execution_policy_multi_threaded::join_results()
    {
    }
}