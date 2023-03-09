#include <iostream>
#include <vector>
#include <mutex>
#include <numeric>
#include <memory>
#include <fstream>

#include "gtest/gtest.h"
#include "execution_policy_multi_threaded.h"
#include "../io_engine/io_engine.h"
#include "../io_engine/io_engine_ios.h"

namespace pitchstream
{
    namespace
    {
        const std::string SAMPLE_DATA_PATH = "../../sample_data/pitch_example_data";
        const std::string SAMPLE_ERROROUS_PATH = "../../sample_data/pitch_errorous_data";
        const std::string EXPECTED_OUTPUT(
            "OIH    5000\n"
            "SPY    2000\n"
            "DRYS   1209\n"
            "ZVZZT   577\n"
            "AAPL    495\n"
            "UYG     400\n"
            "PTR     400\n"
            "FXP     320\n"
            "DIA     229\n"
            "BAC     210\n");
        class execution_policy_multi_threaded_test : public ::testing::Test
        {
        };

        TEST_F(execution_policy_multi_threaded_test, single_child_thread)
        {
            std::ifstream ifile(SAMPLE_DATA_PATH);
            std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_ios(ifile));
            std::unique_ptr<pitchstream::execution_policy> ep(new execution_policy_multi_threaded(1));
            testing::internal::CaptureStdout();

            ep->set_io_engine(ioe.get());
            ep->set_num_results(10);
            ep->run();
            ep.reset();
            ifile.close();
            std::cout.flush();
            std::string text = testing::internal::GetCapturedStdout();

            EXPECT_EQ(text, EXPECTED_OUTPUT);
        }

        TEST_F(execution_policy_multi_threaded_test, many_child_threads)
        {
            std::ifstream ifile(SAMPLE_DATA_PATH);
            std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_ios(ifile));
            std::unique_ptr<pitchstream::execution_policy> ep(new execution_policy_multi_threaded(128));
            testing::internal::CaptureStdout();

            ep->set_io_engine(ioe.get());
            ep->set_num_results(10);
            ep->run();
            ep.reset();
            ifile.close();
            std::cout.flush();
            std::string text = testing::internal::GetCapturedStdout();

            EXPECT_EQ(text, EXPECTED_OUTPUT);
        }
        
        TEST_F(execution_policy_multi_threaded_test, error_handling)
        {
            std::ifstream ifile(SAMPLE_ERROROUS_PATH);
            std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_ios(ifile));
            std::unique_ptr<pitchstream::execution_policy> ep(new execution_policy_multi_threaded(128));
            testing::internal::CaptureStderr();

            ep->set_io_engine(ioe.get());
            ep->set_num_results(10);
            ep->run();
            ep.reset();
            ifile.close();
            std::cerr.flush();
            std::string text = testing::internal::GetCapturedStderr();

            EXPECT_EQ(text, std::string("Following errors were detected during execution:\n"
                                        "1 parse errors detected and ignored\n"
                                        "5 processing errors detected and ignored\n"
                                        "15 input lines skipped\n"));
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
