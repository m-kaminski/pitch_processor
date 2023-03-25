#ifndef _EXECUTION_POLICY_MT_H
#define _EXECUTION_POLICY_MT_H

#include <deque>
#include <mutex>
#include <condition_variable>
#include <numeric>
#include <cassert>
#include <atomic>

#include "../pitch/event_accumulator.h"
#include "../pitch/pitch_format_constants.h"
#include "execution_policy.h"
#include "worker_thread.h"
#include "error_counter.h"
namespace pitchstream
{

    class execution_policy_multi_threaded : public execution_policy
    {
    public:
        execution_policy_multi_threaded(int _num_threads = 32) : 
        num_threads(_num_threads), 
        pre_input_max_length(256*1024),
        thread_data(_num_threads),
        forced_affinity(false){}
        
        virtual void run();
        void set_affinity(bool a) {
            forced_affinity = a;
        }
        bool get_affinity() {
            return forced_affinity;
        }
        int get_num_threads() {
            return num_threads;
        }
    private:
        struct thread_status
        {
            thread_status() {}
            event_accumulator a;
            std::vector<std::string> inputs;
            std::string pre_input;
            std::mutex queue_mutex;
            std::condition_variable mutex_condition;
            int pre_input_length_cap;
            ~thread_status() ;
        };
        int num_threads;
        int pre_input_max_length;
        bool forced_affinity;
        std::vector<thread_status> thread_data;

        error_counter<std::atomic<std::uint64_t>> errors;
        uint64_t lock_fail;
        uint64_t lock_success;

        void process_input_stage1(const char *begin, const char *end);

        void process_input_stage2(worker_thread *w);

        void join_results();
    };
}
#endif
