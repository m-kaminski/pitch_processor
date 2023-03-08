#include <iostream>
#include <string>
#include <sstream>
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

namespace pitchstream
{

    const std::string app_config::help_message(
        "Program parses stream of trade messages in PITCH format (Order Add,\n"
        "Order Execute, Order Cancel, Trade) and calculates list of most frequently\n"
        "executed symbols\n\n"
        "Program accepts following commandline options:\n"
        "-mt[=N] : multi-threaded execution. If =N given, specific number of threads\n"
        "-st : single-threaded execution (default)\n"
        "-aio[=bufsz,inflight] : use Linux AIO for input. Can select buffer size\n"
        "                        (kB) and maximum number of requests in flight (this\n"
        "                        shouldn\'t be set above 2 for pipes)\n"
        "-ios : use io-streams for input (default option)\n"
        "-verbose : print options selected\n"
        "-nr[=N] : number of symbols to calculate (default: 10)\n"
        "-v : print info about applied options (in order)\n\n"
        "-h : display this very help message\n\n"
        "(c) Maciej (Matt) Kaminski\n");

    void app_config::print_help(const char *name)
    {
        std::cout << name << "\n"
                  << help_message << std::endl;
    }
    int app_config::parse_command_line(int argc, const char * const *argv)
    {
        std::stringstream applied;
        bool verbose = false;
        for (int i = 1; i != argc; ++i)
        {
            std::string argument(argv[i]);
            auto it = std::find(argument.begin(), argument.end(), '=');
            auto argname = std::string(argument.begin(), it);
            auto argparms = std::string(it, argument.end());
            if (argname == "-st")
            {
                ep.reset(new pitchstream::execution_policy_single_threaded);
                applied << "Configuring single threaded execution" << std::endl;
            }
            else if (argname == "-nr")
            {

                num_results = stoi(std::string(next(it), argument.end()));
                if (num_results == 0)
                {
                    std::cerr << "Can't process number of results"
                              << std::endl;
                    return (-1);
                }
                applied << "Configuring display of " << num_results << " results " << std::endl;
            }
            else if (argname == "-mt")
            {

                int threads = 0;
                if (argparms.empty())
                {
                    threads = std::thread::hardware_concurrency();
                }
                else
                {
                    threads = stoi(std::string(next(it), argument.end()));
                }
                if (threads == 0)
                {
                    std::cerr << "Can't determine number of threads. Multithreaded "
                                 "execution canceled"
                              << std::endl;
                    return (-1);
                }
                applied << "Configuring " << threads << " threads\n";
                ep.reset(new pitchstream::execution_policy_multi_threaded(threads));
            }
            else if (argname == "-aio")
            {

                if (argparms.empty())
                {
                    ioe.reset(new pitchstream::io_engine_aio(0, 32 * 1024, 2));
                    applied << "Configuring AIO with defaults (32k, 2 buffers)" << std::endl;
                }
                else
                {
                    auto comma = std::find(argparms.begin(), argparms.end(), ',');

                    int bufsz = stoi(std::string(next(argparms.begin()), comma));
                    int inflight;
                    if (comma != argparms.end())
                        inflight = stoi(std::string(next(comma), argparms.end()));
                    else
                        inflight = 2;

                    if (inflight == 0 || bufsz == 0 || inflight % 2)
                    {
                        std::cerr << "Incorrect settings. Need >0 requests, >0 buffer "
                                     "and even number of requests"
                                  << std::endl;
                        return(-1);
                    }
                    applied << "Configuring AIO with " << bufsz << "kB buffer and "
                            << inflight << " requests in flight.\n";
                    bufsz *= 1024;
                    ioe.reset(new pitchstream::io_engine_aio(0, bufsz, inflight));
                }
            }
            else if (argname == "-ios")
            {
                applied << "Configuring IOS" << std::endl;
                ioe.reset(new pitchstream::io_engine_ios(std::cin));
            }
            else if (argname == "-v")
            {
                applied << "Configuring verbose" << std::endl;
                verbose = true;
            }
            else if (argname == "-h")
            {
                print_help(argv[0]);
                return (0);
            }
            else
            {
                std::cerr << "Unknown argument " << argname << std::endl;
                print_help(argv[0]);
                return (-1);
            }
        };
        if (verbose) {
            std::cout << applied.str();
        }
        return 0;
    }

}

#include "app_config.h"