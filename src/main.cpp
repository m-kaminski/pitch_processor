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

namespace pitchstream
{
    void process_input(pitchstream::io_engine & io)
    {
        event_accumulator a;
        io.process_input([&](const char * B, const char * E){
            a.process_message(std::move(
                pitch_decoder::decode(B,E)));
        });

        format_summary(std::cout, a.generate_summary_n(10));
    }
}

int main(int argc, char **argv)
{
    std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_ios(std::cin));

    if (argc > 1) {
        if (std::string(argv[1])=="-aio") {
            ioe.reset(new pitchstream::io_engine_aio(0, 1024*32, 16));
        }

    }

    pitchstream::process_input(*ioe);

    return 0;    
}