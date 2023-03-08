#include "gtest/gtest.h"

#include "io_engine.h"
#include "io_engine_ios.h"
#include "io_engine_aio.h"
#include <iostream>
#include <string>
#include <cassert>

namespace pitchstream
{
    namespace
    {
        class io_engine_test : public ::testing::Test
        {
        };

        TEST_F(io_engine_test, instantiate_ios)
        {
            std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_ios(std::cin));
            EXPECT_NE(ioe.get(), nullptr);
        }
        TEST_F(io_engine_test, instantiate_aio)
        {
            std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_aio(0, 16,2));
            EXPECT_NE(ioe.get(), nullptr);
        }

    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
