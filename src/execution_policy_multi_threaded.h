#ifndef _EXECUTION_POLICY_MT_H
#define _EXECUTION_POLICY_MT_H

#include <deque>
#include <mutex>

#include <condition_variable>
#include <numeric>
#include <cassert>
#include "pitch/event_accumulator.h"
#include "pitch/pitch_format_constants.h"
#include "execution_policy.h"
#include "worker_thread.h"
namespace pitchstream
{

    class execution_policy_multi_threaded : public execution_policy
    {
    public:
        execution_policy_multi_threaded(int _num_threads = 32) : num_threads(_num_threads), multistring_length(1024 * 16),
        thread_data(_num_threads) {}

        virtual void run();

    private:
        struct thread_status
        {
            event_accumulator a;
            std::vector<std::string> inputs;
            std::string pre_input;
            std::mutex queue_mutex;
            std::condition_variable mutex_condition;
        };
        int num_threads;
        int multistring_length;

        std::vector<thread_status> thread_data;
        void process_input_stage1(const char *begin, const char *end);

        void process_input_stage2(worker_thread *w);

        void join_results();
    };
}
#endif
