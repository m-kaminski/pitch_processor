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
#include "error_counter.h"
namespace pitchstream
{
    void execution_policy_single_threaded::run()
    {
        error_counter<uint64_t> errors;
        event_accumulator a;
        ioe->process_input([&](const char *begin, const char *end)
                           { 
                            pitch_decoder::p_message event;
                            try {
                                event = pitch_decoder::decode(begin, end);
                                if (!event) {
                                    errors.lines_skipped ++;
                                }
                            } catch (...) {
                                errors.parse_error_counter++;
                            }
                            try {
                                a.process_message(std::move(event)); 
                            } catch (...) {
                                errors.processing_error_counter++;
                            } });

        format_summary(std::cout, a.generate_summary_n(num_results));
        if (errors.has_errors())
            errors.print_errors();
    }
}