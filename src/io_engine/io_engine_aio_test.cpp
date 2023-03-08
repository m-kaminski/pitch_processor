#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <cstdlib>
#include <errno.h>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fstream>
#include <exception>
#include <iterator>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "io_engine.h"
#include "io_engine_aio.h"

namespace pitchstream
{
    namespace
    {
        class io_engine_aio_test : public ::testing::Test
        {
        };

        TEST_F(io_engine_aio_test, read_regular_file)
        {
            using sv = std::vector<std::string>;
            char file_template[] = "/tmp/pitchstream_io_engine_test_XXXXXX";
            mktemp(file_template);
            std::string tmpfile(file_template);

            // create an output file stream
            std::ofstream outfile(tmpfile);
            sv solution({"foo", "bar", "baz", "alpha", "bravo", "charlie"});
            std::copy(solution.begin(), solution.end(), std::ostream_iterator<std::string>(outfile, "\n"));
            outfile.close();
            // create input file stream and invoke reader

            sv output;
            int ifd = open(tmpfile.c_str(), O_RDONLY);
            std::cout << "tmp file is " << tmpfile << " fd " << ifd << "\n";
            EXPECT_NE(ifd, -1);

            std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_aio(ifd, 8, 4));

            ioe->process_input([&](const char *b, const char *e)
                               { output.push_back(std::string(b, e)); });
            close(ifd);

            EXPECT_NE(ioe.get(), nullptr);
            EXPECT_EQ(output, solution);
            unlink(file_template);
        }

        TEST_F(io_engine_aio_test, missing_terminating_newline)
        {
            using sv = std::vector<std::string>;
            char file_template[] = "/tmp/pitchstream_io_engine_test_XXXXXX";
            mktemp(file_template);
            std::string tmpfile(file_template);

            // create an output file stream
            std::ofstream outfile(tmpfile);
            sv solution({"foo", "bar", "baz"});
            outfile << "foo\nbar\nbaz";
            outfile.close();
            // create input file stream and invoke reader

            sv output;
            int ifd = open(tmpfile.c_str(), O_RDONLY);
            std::cout << "tmp file is " << tmpfile << " fd " << ifd << "\n";
            EXPECT_NE(ifd, -1);

            std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_aio(ifd, 8, 4));

            ioe->process_input([&](const char *b, const char *e)
                               { output.push_back(std::string(b, e)); });
            close(ifd);

            EXPECT_NE(ioe.get(), nullptr);
            EXPECT_EQ(output, solution);
            unlink(file_template);
        }

        TEST_F(io_engine_aio_test, read_fifo)
        {
            using sv = std::vector<std::string>;
            char file_template[] = "/tmp/pitchstream_io_engine_test_XXXXXX";
            mktemp(file_template);
            std::string tmpfile(file_template);
            mkfifo(file_template, 0666);

            sv solution({"foo", "bar", "baz", "alpha", "bravo", "charlie"});

            std::thread writer([&]()
                               {
                // create an output file stream
                std::ofstream fifofile(tmpfile);
                std::copy(solution.begin(), solution.end(), std::ostream_iterator<std::string>(fifofile,"\n"));
                fifofile.close(); });
            // create input file stream and invoke reader
            sv output;

            std::thread reader([&]()
                               {
                                   int ifd = open(tmpfile.c_str(), O_RDONLY);
                                   std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_aio(ifd, 2, 2));
                                   EXPECT_NE(ioe.get(), nullptr);

                                   ioe->process_input([&](const char *b, const char *e)
                                                      { output.push_back(std::string(b, e)); });
                                   close(ifd);
                               });
            writer.join(); // first wait to finish writing, otherwise flaky test
            reader.join();
            EXPECT_EQ(output, solution);
            unlink(file_template);
        }

        TEST_F(io_engine_aio_test, read_generated_fifo)
        {
            using sv = std::vector<std::string>;
            char file_template[] = "/tmp/pitchstream_io_engine_test_XXXXXX";
            mktemp(file_template);
            std::string tmpfile(file_template);

            int mf_r = mkfifo(file_template, 0666);
            if (mf_r < 0)
            {
                std::cerr << "failing create named pipe " << file_template << " because " << strerror(errno) << std::endl;
                throw std::runtime_error(strerror(errno));
            }

            std::string fifo_command("for i in `seq 0 9 ` ; do for j in `seq 10` ; do echo -n $i ; done ; echo ; done > ");
            fifo_command += tmpfile;
            sv solution({"0000000000", "1111111111", "2222222222", "3333333333", "4444444444", "5555555555", "6666666666", "7777777777", "8888888888", "9999999999"});

            std::thread writer([&]()
                               { system(fifo_command.c_str()); });
            sv output;

            std::thread reader([&]()
                               {
                                   int ifd = open(tmpfile.c_str(), O_RDONLY);
                                   std::cout << "tmp file is " << tmpfile << " fd " << ifd << "\n";
                                   std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_aio(ifd, 16, 2));
                                   EXPECT_NE(ioe.get(), nullptr);

                                   ioe->process_input([&](const char *b, const char *e)
                                                      { output.push_back(std::string(b, e)); });
                                   close(ifd);
                               });
            writer.join(); // first wait to finish writing, otherwise flaky test
            reader.join();
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
