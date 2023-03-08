#ifndef _IOENGINE_IOS_H
#define _IOENGINE_IOS_H
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <functional>
#include "io_engine.h"
namespace pitchstream
{
    class io_engine_ios : public io_engine
    {

    public:
        io_engine_ios(std::istream &s) : input_stream(s) {}
        void process_input(line_handler handler);

    private:
        std::istream &input_stream;
    };

}
#endif
