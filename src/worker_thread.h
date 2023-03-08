#ifndef _WORKER_THREAD_H
#define _WORKER_THREAD_H
#include <cstdint>
#include <stdexcept>
#include <memory>
#include <vector>
#include <iostream>
#include <thread>
#include <functional>

namespace pitchstream
{
    class worker_thread
    {
    public:
        using p_t = worker_thread *;
        using run_function_type = std::function<void(worker_thread *)>;

        /***
         *
         *  default constructor
         */
        worker_thread();

        /***
         * create number of workers including self
         */
        void run_with_children(int count);
        
        void join_with_children();

        int get_id()
        {
            return thread_id;
        }
        
        int get_num_workers()
        {
            return worker_vector.size();
        }

        p_t get_worker(int id)
        {
            return worker_vector[id];
        }

        void set_run_function(run_function_type _run_function)
        {
            run_function = _run_function;
        }

    private:
        run_function_type run_function;

        std::unique_ptr<std::thread> p_thread_obj;

        bool running;
        int thread_id;
        std::vector<p_t> children; // empty unless thread_id is 0, in which case holds pointers to all the children
        std::vector<p_t> &worker_vector;

        // replace this function to achieve expected behavior
        static void default_run_function(worker_thread *self);

        worker_thread(int id, std::vector<p_t> &_worker_vector,
                      run_function_type _run_function);

        void run();

        void join(worker_thread *other);

        void run_with_children(int count, std::vector<p_t> &_worker_vector);

        void join_with_children(int count, std::vector<p_t> &_worker_vector);
    };

}
#endif
