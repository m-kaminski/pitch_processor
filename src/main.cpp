#include <iostream>
#include <string>
#include <fstream>

#include "summary_fotmatter.h"
#include "pitch_message.h"
#include "pitch_decoder.h"
#include "event_accumulator.h"

namespace pitchstream {
    /***
     * Process data in PITCH format from standard input,
     * print summary to standard output
    */
    void process_input() {
        event_accumulator a;
        std::string s;
        while (std::getline(std::cin, s)) {
            // for each event
            auto ev = pitch_decoder::decode(s.begin(), s.end());
            a.process_message(std::move(ev));
        }
        format_summary(std::cout , a.generate_summary_n(10));
    }
}

int main(int argc, char **argv)
{
    pitchstream::process_input();
}