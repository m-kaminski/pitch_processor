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
        uint64_t parse_error_counter = 0;
        uint64_t processing_error_counter = 0;
        uint64_t lines_skipped = 0;
        event_accumulator a;
        ioe->process_input([&](const char *begin, const char *end)
                           { 
                            pitch_decoder::p_message event;
                            try {
                                event = pitch_decoder::decode(begin, end);
                                if (!event) {
                                    lines_skipped ++;
                                }
                            } catch (...) {
                                parse_error_counter++;
                            }
                            try {
                                a.process_message(std::move(event)); 
                            } catch (...) {
                                processing_error_counter++;
                            } });

        format_summary(std::cout, a.generate_summary_n(num_results));

        if (parse_error_counter || processing_error_counter || lines_skipped)
        {
            if (parse_error_counter)
            {
                std::cerr << parse_error_counter << " parse errors detected and ignored" << std::endl;
            }
            if (processing_error_counter)
            {
                std::cerr << processing_error_counter << " parse errors detected and ignored" << std::endl;
            }
            if (lines_skipped)
            {
                std::cerr << lines_skipped << " input lines skipped" << std::endl;
            }
        }
    }
}