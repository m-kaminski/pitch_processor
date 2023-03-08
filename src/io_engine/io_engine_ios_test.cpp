#include "gtest/gtest.h"

#include "io_engine.h"
#include "io_engine_ios.h"
#include <iostream>
#include <string>
#include <cstdlib>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fstream>
#include <iterator>


namespace pitchstream
{
    namespace
    {
        class io_engine_ios_test : public ::testing::Test
        {
        };

        TEST_F(io_engine_ios_test, read_regular_file)
        {
            using sv = std::vector<std::string>;
            char file_template[] = "/tmp/pitchstream_io_engine_test_XXXXXX";
            mkstemp(file_template);
            std::string tmpfile(file_template);

            // create an output file stream
            std::ofstream fifofile(tmpfile);
            sv solution({"foo", "bar", "baz"});
            std::copy(solution.begin(), solution.end(), std::ostream_iterator<std::string>(fifofile,"\n"));
            fifofile.close();
            // create input file stream and invoke reader
            sv output;
            std::ifstream infile(tmpfile);
            std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_ios(infile));

            ioe->process_input([&](const char *b, const char *e){output.push_back(std::string(b,e));});

            EXPECT_NE(ioe.get(), nullptr);
            EXPECT_EQ(output, solution);
            unlink(file_template);
        }

    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
