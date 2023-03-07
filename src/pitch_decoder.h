#ifndef _PITCH_DECODE_H
#define _PITCH_DECODE_H
#include <memory>
#include "pitch_message.h"
namespace pitchstream
{

    class pitch_decoder {


    public:
        using p_message = std::unique_ptr<pitch_message>;
        template<typename T>
        static p_message decode(const T & begin, const T & end) {
            if (end - begin <= 9)
                return p_message();
            switch( *(begin+9)) {
                case 'P':
                    // trade
                    break;
                case 'E':
                    // executed
                    break;
                case 'X':
                    // cancel
                    break;
                case 'A':
                    // add
                    break;
            }
            return p_message();
        }
    };
}
#endif