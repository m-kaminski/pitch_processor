#ifndef _IOENGINE_H
#define _IOENGINE_H
#include <cstdint>
#include <stdexcept>
#include <functional>
namespace pitchstream
{
    class io_engine {

        public:

        using line_handler = std::function<void(const char *, const char *)>;

        virtual void process_input(line_handler handler) = 0;
    };

}
#endif
