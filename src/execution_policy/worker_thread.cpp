#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <iterator>
#ifdef __linux__ 
#include <pthread.h>
#include <unistd.h>
#endif
#include "worker_thread.h"

namespace pitchstream
{
    /***
     *
     *  default constructor
     */
    worker_thread::worker_thread() : thread_id(0),
                                     running(false),
                                     children(1, p_t(this)),
                                     worker_vector(children)
    {
        run_function = std::bind(worker_thread::default_run_function, std::placeholders::_1);
    }

    /***
     * create number of workers including self
     */
    void worker_thread::run_with_children(int count, bool affinity)
    {
        children.resize(count); // all children + self

        run_with_children(count, children, affinity);
    }
    
    void worker_thread::join_with_children()
    {
        join_with_children(children.size(), children);

        children.resize(1);
    }

    // replace this function to achieve expected behavior
    void worker_thread::default_run_function(worker_thread *self)
    {
    }
    worker_thread::worker_thread(int id, std::vector<p_t> &_worker_vector,
                                 run_function_type _run_function) : thread_id(id),
                                                                    running(false),
                                                                    worker_vector(_worker_vector),
                                                                    run_function(_run_function)
    {
    }

    void worker_thread::run(bool affinity)
    {
        running = true;
        // run thread with given function
        p_thread_obj.reset(new std::thread([&]()
                                           { run_function(this); }));
#ifdef __linux__ 
    if (affinity) {
        pthread_t thread_native = p_thread_obj->native_handle();
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(get_id() + 1, &cpuset);
        pthread_setaffinity_np(thread_native, sizeof(cpuset), &cpuset);
    }
#endif      
    }

    void worker_thread::join(worker_thread *other)
    {
        other->p_thread_obj->join();
        other->p_thread_obj.reset();
    }

    void worker_thread::run_with_children(int count, std::vector<p_t> &_worker_vector, bool affinity)
    {
        for (int i = 0; i != count; ++i)
        {
            p_t t(new worker_thread(i, _worker_vector, run_function));     
            worker_vector[i] = t;
            t->run(affinity);
        }
#ifdef __linux__ 
    if (affinity) {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(0, &cpuset);
        pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset);
    }
    nice(-1); // elevate priority of main thread slightly above worker threads
#endif    
    }

    void worker_thread::join_with_children(int count, std::vector<p_t> &_worker_vector)
    {
        for (int i = 0; i != count; ++i)
        {
            join(worker_vector[i]);
            delete worker_vector[i];
        }
    }

}
