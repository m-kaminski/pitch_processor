#include <iostream>
#include <vector>
#include <mutex>
#include <numeric>

#include "gtest/gtest.h"
#include "app_config.h"

#include "io_engine/io_engine.h"
#include "io_engine/io_engine_ios.h"
#include "io_engine/io_engine_aio.h"
#include "execution_policy/execution_policy.h"
#include "execution_policy/execution_policy_single_threaded.h"
#include "execution_policy/execution_policy_multi_threaded.h"

namespace pitchstream
{
    namespace
    {
        class app_config_test : public ::testing::Test
        {
        };


        TEST_F(app_config_test, create_instances)
        {
            pitchstream::app_config config;
        }

        TEST_F(app_config_test, test_arguments_ep_mt)
        {
            std::vector<const char*> args({"APPNAME", "-mt"});
            app_config config;
            config.parse_command_line(args.size(), &args[0]);
            EXPECT_NE(dynamic_cast<execution_policy_multi_threaded*>
                    (config.get_execution_policy().get()), nullptr);
        }

        TEST_F(app_config_test, test_default_affinity_setting)
        {
            std::vector<const char*> args({"APPNAME", "-mt=43"});
            app_config config;
            config.parse_command_line(args.size(), &args[0]);
            EXPECT_EQ(dynamic_cast<execution_policy_multi_threaded*>
                        (config.get_execution_policy().get())->get_affinity(),
                         false);
        }
        TEST_F(app_config_test, test_non_default_affinity_setting)
        {
            std::vector<const char*> args({"APPNAME", "-mt=43", "-a"});
            app_config config;
            config.parse_command_line(args.size(), &args[0]);
            EXPECT_EQ(dynamic_cast<execution_policy_multi_threaded*>
                        (config.get_execution_policy().get())->get_affinity(),
                         true);
        }

        TEST_F(app_config_test, test_multiple_affinity_setting)
        {
            std::vector<const char*> args({"APPNAME", "-mt=43", "-a", "-a"});
            app_config config;
            EXPECT_NE(0, config.parse_command_line(args.size(), &args[0]));
        }

        TEST_F(app_config_test, test_wrong_affinity_setting)
        {
            std::vector<const char*> args({"APPNAME", "-st", "-a", "-a"});
            app_config config;
            EXPECT_NE(0, config.parse_command_line(args.size(), &args[0]));
        }

        TEST_F(app_config_test, test_arguments_thread_count)
        {
            std::vector<const char*> args({"APPNAME", "-mt=43"});
            app_config config;
            config.parse_command_line(args.size(), &args[0]);
            EXPECT_EQ(dynamic_cast<execution_policy_multi_threaded*>(config.get_execution_policy().get())->get_num_threads(),
                         43);
        }

        TEST_F(app_config_test, test_arguments_ep_st)
        {
            std::vector<const char*> args({"APPNAME", "-st"});
            app_config config;
            config.parse_command_line(args.size(), &args[0]);
            EXPECT_NE(dynamic_cast<execution_policy_single_threaded*>(config.get_execution_policy().get()), nullptr);
        }

        TEST_F(app_config_test, test_arguments_ioe_aio)
        {
            std::vector<const char*> args({"APPNAME", "-aio"});
            app_config config;
            config.parse_command_line(args.size(), &args[0]);
            EXPECT_NE(dynamic_cast<io_engine_aio*>(config.get_io_engine().get()), nullptr);
        }

        TEST_F(app_config_test, test_arguments_ioe_ios)
        {
            std::vector<const char*> args({"APPNAME", "-ios"});
            app_config config;
            config.parse_command_line(args.size(), &args[0]);
            EXPECT_NE(dynamic_cast<io_engine_ios*>(config.get_io_engine().get()), nullptr);
        }

        TEST_F(app_config_test, test_arguments_ioe_aio_buffer_count)
        {
            std::vector<const char*> args({"APPNAME", "-aio=2,6"});
            app_config config;
            config.parse_command_line(args.size(), &args[0]);
            EXPECT_EQ(dynamic_cast<io_engine_aio*>(config.get_io_engine().get())->get_buffer_size(), 2048);
            EXPECT_EQ(dynamic_cast<io_engine_aio*>(config.get_io_engine().get())->get_max_ios_inflight(), 6);
        }

        TEST_F(app_config_test, test_incorrect_aio_spec_point)
        {
            std::vector<const char*> args({"APPNAME", "-aio=2.6"});
            app_config config;
            EXPECT_NE(0, config.parse_command_line(args.size(), &args[0]));
        }

        TEST_F(app_config_test, test_incorrect_aio_spec_first)
        {
            std::vector<const char*> args({"APPNAME", "-aio=2f,6"});
            app_config config;
            EXPECT_NE(0, config.parse_command_line(args.size(), &args[0]));
        }
        
        TEST_F(app_config_test, test_incorrect_aio_spec_second)
        {
            std::vector<const char*> args({"APPNAME", "-aio=2,-6"});
            app_config config;
            EXPECT_NE(0, config.parse_command_line(args.size(), &args[0]));
        }

        TEST_F(app_config_test, test_arguments_ioe_aio_only_buffer_size)
        {
            std::vector<const char*> args({"APPNAME", "-aio=16"});
            app_config config;
            config.parse_command_line(args.size(), &args[0]);
            EXPECT_EQ(dynamic_cast<io_engine_aio*>(config.get_io_engine().get())->get_buffer_size(), 1024*16);
        }


        TEST_F(app_config_test, test_arguments_multiple)
        {
            std::vector<const char*> args({"APPNAME", "-aio", "-mt"});
            app_config config;
            config.parse_command_line(args.size(), &args[0]);
            EXPECT_NE(dynamic_cast<io_engine_aio*>(config.get_io_engine().get()), nullptr);
            EXPECT_NE(dynamic_cast<execution_policy_multi_threaded*>(config.get_execution_policy().get()), nullptr);
        }

        TEST_F(app_config_test, test_incorrect)
        {
            std::vector<const char*> args({"APPNAME", "-aio=0", "-mt"});
            app_config config;
            EXPECT_NE(0, config.parse_command_line(args.size(), &args[0]));
        }

        TEST_F(app_config_test, test_nr)
        {
            std::vector<const char*> args({"APPNAME", "-nr=4"});
            app_config config;
            config.parse_command_line(args.size(), &args[0]);
            EXPECT_EQ(config.get_num_results(), 4);
        }

        TEST_F(app_config_test, test_arguments_multiple_thread_options)
        {
            std::vector<const char*> args({"APPNAME", "-st", "-mt"});
            app_config config;
            EXPECT_NE(0, config.parse_command_line(args.size(), &args[0]));
        }


        TEST_F(app_config_test, test_arguments_multiple_io_options)
        {
            std::vector<const char*> args({"APPNAME", "-aio", "-aio=10"});
            app_config config;
            EXPECT_NE(0, config.parse_command_line(args.size(), &args[0]));
        }

        TEST_F(app_config_test, test_arguments_multiple_result_options)
        {
            std::vector<const char*> args({"APPNAME", "-nr=4", "-nr=10"});
            app_config config;
            EXPECT_NE(0, config.parse_command_line(args.size(), &args[0]));
        }

    }
}
