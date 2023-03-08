#include <iostream>
#include <string>
#include <fstream>
#include "summary_fotmatter.h"
#include "pitch_message.h"
#include "pitch_decoder.h"
#include "event_accumulator.h"


#include <iterator>

#include "io_engine/io_engine.h"
#include "io_engine/io_engine_ios.h"
#include "io_engine/io_engine_aio.h"

// for aio
#include <array>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <aio.h>
#include <memory>
#include <exception>
#include <signal.h>
#include <cstring>
#include <algorithm>
#include <sched.h>
namespace pitchstream
{
    void io_engine_ios::process_input(io_engine::line_handler handler)
    {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(NULL);
        event_accumulator a;
        std::string s;
        while (std::getline(std::cin, s))
        {
            // for each event
            handler(&*s.begin(), &*(s.begin() + s.size()) );
        }
    }

    void count_lines_ios()
    {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(NULL);
        int i = 0;
        std::string s;
        while (std::getline(std::cin, s))
        {
            i++;
        }

        std::cout << i << std::endl;
        ;
    }




    struct io_request
    {
        aiocb cb;
        std::unique_ptr<char> iobuf;
    };

    /***
     * Time to ingest 2000000000 lines:
     * AIO, 32 in flight, 4k buf: 40.55
     * AIO, 16 in flight, 8k buf: 26.19
     * AIO, 32 in flight, 8k buf: 27.75
     * AIO, 16 in flight, 16k buf: 22.11
     * AIO, 16 in flight, 32k buf: 21.36
     * AIO, 8 in flight, 64k buf, 21.79
     * IOS, 42.31
     */

    void io_engine_aio::process_input(io_engine::line_handler handler) {

        const int num_ios_inflight = 8;
        const int buf_sz = 1024*64;
        int cur_io = 0;
        std::array<io_request, num_ios_inflight> io_pool;
        int line_count = 0;

        int ios_inflight = 0;
        int last_io_called = -1;

        for (int i = 0; i != num_ios_inflight; ++i)
        {
            io_pool[i].iobuf.reset(new char[buf_sz]);
            io_pool[i].cb.aio_nbytes = buf_sz;
            io_pool[i].cb.aio_reqprio = 0;
            io_pool[i].cb.aio_fildes = 0; // stdin
            io_pool[i].cb.aio_buf = io_pool[i].iobuf.get();
            io_pool[i].cb.aio_offset = i*buf_sz;
            io_pool[i].cb.aio_sigevent.sigev_notify = SIGEV_NONE;
        }

        std::string carry_line;

        while (true)
        {
            // run some IOs ahead of current one (will only happen over first iteration of the loop)
            if (ios_inflight < num_ios_inflight)
            {
                for (int i = ios_inflight; i < ios_inflight + 2; ++i)
                {
                    int rr = aio_read(&io_pool[i].cb);
                    if (rr == -1)
                    {
                        throw std::runtime_error(strerror(errno));
                    }
                }
                ios_inflight += 2;
            }

            while (aio_error(&io_pool[cur_io].cb) == EINPROGRESS)
                sched_yield(); // busy wait;
            int aio_res = aio_return(&io_pool[cur_io].cb);

            if (aio_res == 0)
            {
                std::cout << "likely eof" << std::endl;
                break;
            }
            if (aio_res < 0)
            {
                std::cout << "likely io error" << std::endl;
                break;
            }

            char * begin = io_pool[cur_io].iobuf.get();
            char * end = io_pool[cur_io].iobuf.get() + aio_res;

            int cctr = 0;
            while (begin < end) {
                char * eln  = std::find(begin, end, '\n');

                if (eln == end ) {
                    // no new line; done for this input
                    carry_line += std::string(begin, end);
                    break;
                }
                if (carry_line.size()) { // there is a carry line from previous input processing
                    carry_line += std::string(begin, eln);
                    handler(&*carry_line.begin(), &*(carry_line.begin() + carry_line.size()) );
                    carry_line.erase();
                } else {
                    handler(begin, eln);
                }
                begin = eln + 1;
            }

            int rr = aio_read(&io_pool[cur_io].cb);

            cur_io = (cur_io + 1) % num_ios_inflight;
        }


    }




    void process_input(pitchstream::io_engine & io)
    {

        event_accumulator a;

        io.process_input([&](const char * B, const char * E){
            //std::cout << "LINE: " << std::string(B,E) << std::endl;
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