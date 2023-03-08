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

        void process_input(line_handler handler);
    };

}
#endif
