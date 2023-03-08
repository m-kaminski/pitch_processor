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
namespace pitchstream
{

    struct thread_status
    {
        event_accumulator a;
        std::vector<std::string> pre_inputs;
        std::vector<std::string> inputs;
        std::mutex queue_mutex;
        std::condition_variable mutex_condition;
        int counter = 0;
    };

    void process_input_mt(pitchstream::io_engine &io)
    {
        const int num_threads = 16;
        std::vector<thread_status> thread_data(num_threads);
        int list_batch_size = 4096;
        worker_thread wt;
        wt.set_run_function([&](worker_thread *w)
                            {
                                int thread_id = w->get_id();

                                while (true)
                                {
                                    bool break_condition = false;
                                    std::vector<std::string> messages;
                                    // std::string message;
                                    {
                                        std::unique_lock<std::mutex> lock(thread_data[thread_id].queue_mutex);
                                        thread_data[thread_id].mutex_condition.wait(lock, [&]
                                                                                    { return !thread_data[thread_id].inputs.empty(); });
                                        // message = thread_data[thread_id].inputs.front();
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
                                            thread_data[thread_id].a.process_message(std::move(
                                                pitch_decoder::decode(message.begin(), message.end())));
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
                                // merging partial results here
                            });

        wt.run_with_children(num_threads);
        io.process_input([&](const char *B, const char *E)
                         {
                             std::string message(B, E);

                             static const int COMMON_ORDER_ID_OFFSET = 10;
                             static const int COMMON_ORDER_ID_LENGTH = 12;
                             uint64_t oid =
                                 std::accumulate(B + COMMON_ORDER_ID_OFFSET,
                                                 B + COMMON_ORDER_ID_OFFSET + COMMON_ORDER_ID_LENGTH,
                                                 0);

                             int thread_id = oid % num_threads;

                             thread_data[thread_id].pre_inputs.push_back(message);

                             if (thread_data[thread_id].pre_inputs.size() > list_batch_size)
                             {                                     
                                thread_data[thread_id].counter += thread_data[thread_id].pre_inputs.size();

                                 {
                                     std::unique_lock<std::mutex> lock(thread_data[thread_id].queue_mutex);


                                     if (thread_data[thread_id].inputs.empty())
                                         thread_data[thread_id].inputs.swap(thread_data[thread_id].pre_inputs);
                                     else
                                         std::move(thread_data[thread_id].pre_inputs.begin(),
                                                   thread_data[thread_id].pre_inputs.end(),
                                                   std::back_inserter(thread_data[thread_id].inputs));
                                 }
                                 thread_data[thread_id].mutex_condition.notify_one();
                                 thread_data[thread_id].pre_inputs.clear();
                             }
                         });
        // end of file, send empty to each thread to terminate

        for (int i = 0; i != num_threads; ++i)
        {

            thread_data[i].counter += thread_data[i].pre_inputs.size();
            std::cout << "Thread " << i << " processed " << thread_data[i].counter << std::endl;
            {
                std::unique_lock<std::mutex> lock(thread_data[i].queue_mutex);
                thread_data[i].inputs.push_back(std::string());
            }
            thread_data[i].mutex_condition.notify_one();
        }

        wt.join_with_children();
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
            ioe.reset(new pitchstream::io_engine_aio(0, 1024 * 32, 16));
        }
    }

    pitchstream::process_input_mt(*ioe);
    // pitchstream::process_input(*ioe);

    return 0;
}