#include <iostream>
#include <string>
#include <fstream>
#include <iterator>

#include "../pitch/summary_fotmatter.h"
#include "../pitch/pitch_message.h"
#include "../pitch/pitch_decoder.h"
#include "../pitch/event_accumulator.h"
#include "worker_thread.h"
#include "execution_policy_single_threaded.h"

namespace pitchstream
{
    void execution_policy_single_threaded::run()
    {

        event_accumulator a;
        ioe->process_input([&](const char *B, const char *E)
                           { a.process_message(std::move(
                                 pitch_decoder::decode(B, E))); });

        format_summary(std::cout, a.generate_summary_n(num_results));
    }
}