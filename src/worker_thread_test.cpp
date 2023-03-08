#include "gtest/gtest.h"

#include "worker_thread.h"
#include <iostream>
#include <vector>
#include <mutex>
#include <numeric>

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
            wt.set_run_function([&](worker_thread *w)
                                {

            std::cout << "running inside thread id " << w->get_id() << "\n";
                std::scoped_lock lock(hello_mutex);
                hello_thread.push_back(w->get_id()); });
            wt.run_with_children(9);
            std::vector<int> expected(10);
            std::iota(expected.begin(), expected.end(), 0);
            std::sort(hello_thread.begin(), hello_thread.end());
            EXPECT_EQ(expected, hello_thread);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
