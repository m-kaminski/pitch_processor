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
        using u_ptr_t = std::unique_ptr<worker_thread>;

        /***
         *
         *  default constructor
         */
        worker_thread() : thread_id(0),
                          running(false),
                          children(1, p_t(this)),
                          worker_vector(children)
        {
            std::cout << "created thread id " << thread_id << "\n";

            join_function = std::bind(worker_thread::default_join_function, std::placeholders::_1, std::placeholders::_2);
            run_function = std::bind(worker_thread::default_run_function, std::placeholders::_1);
        }

        /***
         * create number of workers including self
         */
        void run_with_children(int count)
        {
            if (running)
            {
                return;
            }
            children.resize(count + 1); // all children + self

            run();
            run_with_children(count, children);
            join_with_children(count, children);
            join(this);
        }

        int get_id() { return thread_id; }
        int get_num_workers()
        {
            return worker_vector.size();
        }

        p_t get_worker(int id)
        {
            return worker_vector[id];
        }

        using join_function_type = std::function<void(worker_thread *, worker_thread *)>;
        using run_function_type = std::function<void(worker_thread *)>;

        void set_join_function(join_function_type _join_function)
        {
            join_function = _join_function;
        }

        void set_run_function(run_function_type _run_function)
        {
            run_function = _run_function;
        }

    private:
        // replace this function to achieve expected behavior
        static void default_run_function(worker_thread *self)
        {
            std::cout << "running thread id " << self->thread_id << "\n";
        }

        static void default_join_function(worker_thread *self, worker_thread *other)
        {
            std::cout << "joined thread " << other->get_id() << " to " << self->get_id() << "\n";
        }

        join_function_type join_function;
        run_function_type run_function;

        worker_thread(int id, std::vector<p_t> &_worker_vector,
                      join_function_type _join_function,
                      run_function_type _run_function) : thread_id(id),
                                                         running(false),
                                                         worker_vector(_worker_vector),
                                                         join_function(_join_function),
                                                         run_function(_run_function)
        {
            std::cout << "created thread id " << id << "/" << worker_vector.size() << " (c)\n";
        }

        void run()
        {
            running = true;
            // run thread with given function
            p_thread_obj.reset(new std::thread([&]()
                                               { run_function(this); }));
        }

        void join(worker_thread *other)
        {
            other->p_thread_obj->join();
            join_function(this, other);   
        }

        std::unique_ptr<std::thread> p_thread_obj;

        bool running;
        int thread_id;
        std::vector<p_t> children; // empty unless thread_id is 0, in which case holds pointers to all the children
        std::vector<p_t> &worker_vector;

        void run_with_children(int count, std::vector<p_t> &_worker_vector)
        {
            std::cout << "Creating " << count << " children " << thread_id << std::endl;
            if (count == 0)
            {
                ;
            }
            else if (count == 1)
            {
                p_t t(new worker_thread(thread_id + 1, _worker_vector, join_function, run_function));
                worker_vector[thread_id + 1] = t;
                t->run();
            }
            else
            {
                int lower_half = count / 2;
                int upper_half = count - lower_half;

                p_t t(new worker_thread(thread_id + lower_half, _worker_vector, join_function, run_function));
                worker_vector[thread_id + lower_half] = t;                
                t->run();

                // recursively
                run_with_children(lower_half - 1, _worker_vector);
                t->run_with_children(upper_half, _worker_vector);   
            }
        }

        void join_with_children(int count, std::vector<p_t> &_worker_vector)
        {

            if (count == 0)
            {
                ;
            }
            else if (count == 1)
            {
                join(worker_vector[thread_id + 1]);
            }
            else
            {
                int lower_half = count / 2;
                int upper_half = count - lower_half;
                // create children
                join_with_children(lower_half - 1, _worker_vector);
                worker_vector[thread_id + lower_half]->join_with_children(upper_half, _worker_vector);
                
                join(worker_vector[thread_id + lower_half]);
            }

        }
    };

}
#endif
