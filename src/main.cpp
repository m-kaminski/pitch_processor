#include <iostream>
#include <string>
#include <fstream>
#include <iterator>

#include "pitch/summary_fotmatter.h"
#include "pitch/pitch_message.h"
#include "pitch/pitch_decoder.h"
#include "pitch/event_accumulator.h"
#include "io_engine/io_engine.h"
#include "io_engine/io_engine_ios.h"
#include "io_engine/io_engine_aio.h"
#include "worker_thread.h"

#include <deque>
#include <mutex>
#include "pitch/base36.h"

#include <condition_variable>
#include <numeric>
#include <cassert>
#include "pitch/pitch_format_constants.h"
namespace pitchstream
{

    struct thread_status
    {
        event_accumulator a;
        std::vector<std::string> inputs;
        std::string pre_input;
        std::mutex queue_mutex;
        std::condition_variable mutex_condition;
    };

    // try large character buffers (multiple strings) on input size
    void process_input_mt(pitchstream::io_engine &io)
    {
        const int num_threads = 32;
        std::vector<thread_status> thread_data(num_threads);
        int multistring_length = 1024 * 16;
        worker_thread wt;


        for (int i = 0; i != num_threads; ++i)
            thread_data[i].pre_input.reserve(multistring_length + 100);
        wt.set_run_function([&](worker_thread *w)
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
                                            while (middle != end) {
                                                auto new_middle = std::find(middle,end, '\n');

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
                                } });

        wt.run_with_children(num_threads);
        io.process_input([&](const char *B, const char *E)
                         {
                             int thread_id =
                                 std::accumulate(B + COMMON_ORDER_ID_OFFSET,
                                                 B + COMMON_ORDER_ID_OFFSET + COMMON_ORDER_ID_LENGTH,
                                                 0) %
                                 num_threads;

                             thread_data[thread_id].pre_input.append(B, E);
                             thread_data[thread_id].pre_input += '\n';

                             if (thread_data[thread_id].pre_input.size() > multistring_length)
                             {
                                 {
                                     std::unique_lock<std::mutex> lock(thread_data[thread_id].queue_mutex);

                                     thread_data[thread_id].inputs.emplace_back(move(thread_data[thread_id].pre_input));
                                 }
                                 thread_data[thread_id].mutex_condition.notify_one();
                                 thread_data[thread_id].pre_input.reserve(multistring_length + 100);
                             } });
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

        for (int i = 1; i != num_threads; ++i)
        {
            thread_data[0].a.add(thread_data[i].a);
        }
        format_summary(std::cout, thread_data[0].a.generate_summary_n(10));
    }

    void process_input(pitchstream::io_engine &io)
    {
        event_accumulator a;
        io.process_input([&](const char *B, const char *E)
                         { a.process_message(std::move(
                               pitch_decoder::decode(B, E))); });

        format_summary(std::cout, a.generate_summary_n(10));
    }
}

int main(int argc, char **argv)
{
    std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_ios(std::cin));

    if (argc > 1)
    {
        if (std::string(argv[1]) == "-aio")
        {
            ioe.reset(new pitchstream::io_engine_aio(0, 1024 * 32, 2));
        }
    }

    pitchstream::process_input_mt(*ioe);
    // pitchstream::process_input(*ioe);

    return 0;
}