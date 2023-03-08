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
#include "worker_thread.h"
#include "execution_policy.h"
#include "execution_policy_single_threaded.h"
#include "execution_policy_multi_threaded.h"


int main(int argc, char **argv)
{
    std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_ios(std::cin));

    if (argc > 1)
    {
        if (std::string(argv[1]) == "-aio")
        {
            ioe.reset(new pitchstream::io_engine_aio(0, 1024 * 32, 2));
        }
    }

    //std::unique_ptr<pitchstream::execution_policy> ep (new pitchstream::execution_policy_single_threaded);
    std::unique_ptr<pitchstream::execution_policy> ep (new pitchstream::execution_policy_multi_threaded);

    ep->set_io_engine(ioe.get());
    ep->run();

    //pitchstream::process_input_mt(*ioe);
    // pitchstream::process_input(*ioe);

    return 0;
}