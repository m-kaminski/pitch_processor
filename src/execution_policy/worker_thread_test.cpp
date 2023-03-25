#include <iostream>
#include <vector>
#include <mutex>
#include <numeric>

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
            wt.run_with_children(9, false);
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
            wt.run_with_children(10, true);
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
            wt.run_with_children(num_threads, false);
            wt.join_with_children();
            std::vector<int> expected(num_threads);
            std::iota(expected.begin(), expected.end(), 0);
            // no sort unlike previous test
            EXPECT_EQ(expected, hello_thread);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
