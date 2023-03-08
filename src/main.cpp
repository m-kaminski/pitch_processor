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

const std::string help_message(
   "Program parses stream of trade messages in PITCH format (Order Add,\n"
   "Order Execute, Order Cancel, Trade) and calculates list of most frequently\n"
   "executed symbols\n\n"
   "Program accepts following commandline options:\n"
   "-mt[=N] : multi-threaded execution. If =N given, specific number of threads"
   "-st : single-threaded execution (default)\n"
   "-aio[=bufsz,inflight] : use Linux AIO for input. Can select buffer size\n"
   "                        (kB) and maximum number of requests in flight (this\n"
   "                        shouldn\'t be set above 2 for pipes)"
   "-ios : use io-streams for input (default option)\n"
   "-verbose : print options selected\n"
   "-h : display this very help message\n\n"
   "(c) Maciej (Matt) Kaminski\n"
);

void print_help(char * name) {
    std::cout << name << "\n" << help_message << std::endl;
}

int main(int argc, char **argv)
{
    std::unique_ptr<pitchstream::io_engine> ioe;
    std::unique_ptr<pitchstream::execution_policy> ep;
    
    // parse command line
    std::for_each(argv+1, argv+argc, [&](char* _argument){
        std::string argument(_argument);
        auto it = std::find(argument.begin(), argument.end(), '=');
        auto argname = std::string(argument.begin(), it);
        auto argparms = std::string(it, argument.end());
        if (argname == "-st") {
            ep.reset(new pitchstream::execution_policy_single_threaded);
        } else if (argname == "-mt") {
            int threads = 0;
            if (argparms.empty()) {
                threads = std::thread::hardware_concurrency();
            } else {
                threads = stoi(std::string(next(it),argument.end()));
            }
            if (threads == 0) {
                std::cerr << "Can't determine number of threads. Multithreaded "
                             "execution canceled" << std::endl;
                exit(-1);
            }
            std::cout << "Configuring " << threads << " threads\n";
            ep.reset(new pitchstream::execution_policy_multi_threaded(threads));

        } else if (argname == "-aio") {

            if (argparms.empty()) {
                ioe.reset(new pitchstream::io_engine_aio(0, 32*1024, 2));
                std::cout << "Configuring AIO with" << std::endl;
            } else {
                auto comma = std::find(argparms.begin(), argparms.end(), ',');

                int bufsz = stoi(std::string(next(argparms.begin()),comma));
                int inflight;
                if (comma != argparms.end())
                    inflight = stoi(std::string(next(comma),argparms.end()));
                else
                    inflight = 2;

                if (inflight == 0 || bufsz == 0 || inflight % 2) {
                    std::cerr << "Incorrect settings. Need >0 requests, >0 buffer "
                                 "and even number of requests" << std::endl;
                    exit(-1);
                }
                std::cout << "Configuring AIO with " << bufsz << "kB buffer and "
                << inflight << " requests in flight.\n";
                bufsz *= 1024;
                ioe.reset(new pitchstream::io_engine_aio(0, bufsz, inflight));

            } 

        } else if (argname == "-ios") {
            ioe.reset(new pitchstream::io_engine_ios(std::cin));
        } else if (argname == "-h") {
            print_help(argv[0]);
            exit(0);
        } else {
            std::cerr << "Unknown argument " << argname << std::endl;
            print_help(argv[0]);
            exit(-1);
        }

    });

    if (!ioe)
        ioe.reset(new pitchstream::io_engine_ios(std::cin));

    if (!ep)
        ep.reset(new pitchstream::execution_policy_single_threaded);

    // run
    ep->set_io_engine(ioe.get());
    ep->run();

    return 0;
}