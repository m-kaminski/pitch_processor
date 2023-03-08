#include <iostream>
#include <string>
#include <fstream>
#include <iterator>

#include "pitch/summary_fotmatter.h"
#include "pitch/pitch_message.h"
#include "pitch/pitch_decoder.h"
#include "pitch/event_accumulator.h"
#include "io_engine/io_engine.h"
#include "io_engine/io_engine_ios.h"
#include "io_engine/io_engine_aio.h"
#include "execution_policy/execution_policy.h"
#include "execution_policy/execution_policy_single_threaded.h"
#include "execution_policy/execution_policy_multi_threaded.h"
#include "app_config.h"

int main(int argc, char **argv)
{
    // application settings
    pitchstream::app_config config;
    int config_res = config.parse_command_line(argc, argv);
    if (config_res) {
        return config_res > 0 ? 0 : config_res;
    }

    // Information parsed from command line
    std::shared_ptr<pitchstream::io_engine> ioe(config.get_io_engine());
    std::shared_ptr<pitchstream::execution_policy> ep(config.get_execution_policy());
    int num_results=config.get_num_results();

    // set defaults if not set
    if (!ioe)
        ioe.reset(new pitchstream::io_engine_ios(std::cin));

    if (!ep)
        ep.reset(new pitchstream::execution_policy_single_threaded);

    // run
    ep->set_io_engine(ioe.get());
    ep->set_num_results(num_results);
    ep->run();
    return 0;
}