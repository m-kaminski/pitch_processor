#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <cassert>
#include "error_counter.h"

namespace pitchstream
{
    namespace
    {
        class error_counter : public ::testing::Test
        {
        };

        TEST_F(error_counter, negative_test)
        {
            pitchstream::error_counter<uint64_t> counter;
            EXPECT_FALSE(counter.has_errors());
        }
        TEST_F(error_counter, positive_test)
        {
            pitchstream::error_counter<uint64_t> counter;
            counter.lines_skipped++;
            EXPECT_TRUE(counter.has_errors());
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
