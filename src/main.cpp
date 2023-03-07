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
            a.process_message(std::move(
                pitch_decoder::decode(s.begin(), s.end())));
        }
        format_summary(std::cout , a.generate_summary_n(10));
    }
}

int main(int argc, char **argv)
{
    std::ios_base::sync_with_stdio (false);
    std::cin.tie(NULL);
    pitchstream::process_input();
}