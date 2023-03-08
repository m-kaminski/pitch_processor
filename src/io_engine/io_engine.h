#ifndef _IOENGINE_H
#define _IOENGINE_H
#include <cstdint>
#include <stdexcept>
#include <functional>
namespace pitchstream
{
    /**
     * Abstract interface class allowing for multiple ways of processing input (i.e. streams or
     * linux AIO).
    */
    class io_engine {

        public:

        /**
         * Every client will have to provide function that will be invoked for each
         * line; It takes begin/end of character string in same manner as C++ iterator
         * work (so begin is index 0, end is one after last)
        */
        using line_handler = std::function<void(const char *, const char *)>;

        /**
         * Process input according to io_engine policy 
        */
        virtual void process_input(line_handler handler) = 0;
    };

}
#endif
