#ifndef _IOENGINE_AIO_H
#define _IOENGINE_AIO_H
#include <cstdint>
#include <stdexcept>
#include <functional>
#include "io_engine.h"
namespace pitchstream
{
    class io_engine_aio : public io_engine {
        
        public:
        io_engine_aio(int input_fd, int buffer_size, int num_ios_inflight);
        void process_input(line_handler handler);

        private:
        // file descriptor, size of read buffer and max number of ios in flight (must be divisible by 2)
        int input_fd;
        off_t buffer_size;
        int num_ios_inflight;
    };

}
#endif
