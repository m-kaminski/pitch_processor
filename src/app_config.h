#ifndef _APP_CONFIG_H
#define _APP_CONFIG_H
#include <cstdint>
#include <stdexcept>
#include <functional>
#include <memory>

#include "io_engine/io_engine.h"
#include "execution_policy/execution_policy.h"

namespace pitchstream
{
    class app_config
    {

    public:
        app_config() : num_results(10) {}

        int parse_command_line(int argc, const char * const * argv);

        std::shared_ptr<io_engine> get_io_engine()
        {
            return ioe;
        }
        std::shared_ptr<execution_policy> get_execution_policy()
        {
            return ep;
        }
        int get_num_results()
        {
            return num_results;
        }

        bool is_valid_config();

    private:
        void print_help(const char *name);

        std::shared_ptr<io_engine> ioe;
        std::shared_ptr<execution_policy> ep;
        int num_results;
        static const std::string help_message;
    };

}
#endif
