#ifndef _IOENGINE_IOS_H
#define _IOENGINE_IOS_H
#include <cstdint>
#include <stdexcept>
#include <functional>
#include "io_engine.h"
namespace pitchstream
{
    class io_engine_ios : public io_engine {

        public:

        void process_input(line_handler handler);
    };

}
#endif
