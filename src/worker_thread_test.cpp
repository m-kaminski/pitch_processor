#include <iostream>
#include <vector>
#include <mutex>
#include <numeric>
#include <time.h>
#include "gtest/gtest.h"
#include "worker_thread.h"

namespace pitchstream
{
    namespace
    {
        class worker_thread_test : public ::testing::Test
        {
        };

        TEST_F(worker_thread_test, create_instances)
        {

            std::vector<int> hello_thread;
            std::mutex hello_mutex;

            worker_thread wt;
            wt.run_with_children(9);
            wt.join_with_children();
        }
        
        TEST_F(worker_thread_test, access_shared_resources)
        {

            std::vector<int> hello_thread;
            std::mutex hello_mutex;

            worker_thread wt;
            wt.set_run_function([&](worker_thread *w)
                                {           std::cout << "running inside thread id " << w->get_id() << "\n";
                std::scoped_lock lock(hello_mutex);
                hello_thread.push_back(w->get_id()); });
            wt.run_with_children(10);
            wt.join_with_children();
            std::vector<int> expected(10);
            std::iota(expected.begin(), expected.end(), 0);
            std::sort(hello_thread.begin(), hello_thread.end());
            EXPECT_EQ(expected, hello_thread);
        }

        TEST_F(worker_thread_test, addressing_num_thread)
        {

            int num_threads = 17;
            std::vector<int> hello_thread(17);
            std::mutex hello_mutex;

            worker_thread wt;
            wt.set_run_function([&](worker_thread *w)
                                {           std::cout << "running inside thread id " << w->get_id() << "\n";
                hello_thread[w->get_id()] = w->get_id(); });
            wt.run_with_children(num_threads);
            wt.join_with_children();
            std::vector<int> expected(num_threads);
            std::iota(expected.begin(), expected.end(), 0);
            // no sort unlike previous test
            EXPECT_EQ(expected, hello_thread);
        }

        TEST_F(worker_thread_test, join_instances)
        {
            int num_threads = 16;
            std::vector<int> hello_thread(num_threads);
            std::mutex hello_mutex;

            worker_thread wt;
            wt.set_run_function([&](worker_thread *w)
                                { std::cout << "running inside thread id " << w->get_id() << "\n";
                hello_thread[w->get_id()] = w->get_id(); });

            wt.set_join_function([&](worker_thread *self, worker_thread *other)
                                 {             
                                    
                                    std::cout << "joined thread " << other->get_id() << " to " << self->get_id() << "\n";
                                    std::scoped_lock lock(hello_mutex);
                                    if (self != other)
                                    hello_thread[self->get_id()] += hello_thread[other->get_id()]; });

            wt.run_with_children(num_threads);
            wt.join_with_children();
            EXPECT_EQ((num_threads) * (num_threads - 1) / 2, hello_thread[0]);
        }

        TEST_F(worker_thread_test, test_with_sleep)
        {
            int num_threads = 16;
            std::vector<int> hello_thread(num_threads);
            std::mutex hello_mutex;
            timespec ts_req({0, 10000});
            timespec ts_rem;
            worker_thread wt;
            wt.set_run_function([&](worker_thread *w)
                                { std::cout << "running inside thread id " << w->get_id() << "\n";
                                nanosleep(&ts_req, &ts_rem);
                hello_thread[w->get_id()] = w->get_id(); });

            wt.set_join_function([&](worker_thread *self, worker_thread *other)
                                 {             
                                    
                                    std::cout << "joined thread " << other->get_id() << " to " << self->get_id() << "\n";
                                    std::scoped_lock lock(hello_mutex);
                                    if (self != other)
                                    hello_thread[self->get_id()] += hello_thread[other->get_id()];
                                nanosleep(&ts_req, &ts_rem); });

            wt.run_with_children(num_threads);
            wt.join_with_children();
            EXPECT_EQ((num_threads) * (num_threads - 1) / 2, hello_thread[0]);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
