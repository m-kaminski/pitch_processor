#include <iostream>
#include <string>
#include <fstream>
#include "pitch/summary_fotmatter.h"
#include "pitch/pitch_message.h"
#include "pitch/pitch_decoder.h"
#include "pitch/event_accumulator.h"


#include <iterator>

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
    std::unique_ptr<pitchstream::io_engine> ioe(new pitchstream::io_engine_ios);

    if (argc > 1) {
        if (std::string(argv[1])=="aio") {
            std::cout << "using asynchronous IO" << std::endl;
            ioe.reset(new pitchstream::io_engine_aio);
        }

    }

    pitchstream::process_input(*ioe);
}