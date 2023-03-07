#ifndef _PITCH_DECODE_H
#define _PITCH_DECODE_H
#include <memory>
#include "pitch_message.h"
namespace pitchstream
{

    class pitch_decoder
    {

    public:
        using p_message = std::unique_ptr<pitch_message>;

        /***
         * Function take ranges of characters (iterators or pointers)
         * and generate a message
         *
         * @param begin iterator to a begining of message to decode
         * @param end iterator to an ending of message to decode
         */
        template <typename T>
        static p_message decode(const T &begin, const T &end)
        {
            const int MESSAGE_TYPE_OFFSET = 9;
            const char MESSAGE_ADD_ORDER = 'A';
            const char MESSAGE_ORDER_CANCEL = 'X';
            const char MESSAGE_ORDER_EXECUTED = 'E';
            const char MESSAGE_TRADE = 'P';

            if (end - begin <= MESSAGE_TYPE_OFFSET)
                return p_message();
            switch (*(begin + MESSAGE_TYPE_OFFSET))
            {
            case MESSAGE_TRADE:
                return decode_trade(begin, end); // trade
            case MESSAGE_ORDER_EXECUTED:
                return decode_executed(begin, end); // executed
            case MESSAGE_ORDER_CANCEL:
                return decode_cancel(begin, end); // cancel
            case MESSAGE_ADD_ORDER:
                return decode_add(begin, end); // add
            }
            return p_message();
        }

    private:
        /**
         * decode and validate Add Order message
         */
        template <typename T>
        static p_message decode_add(const T &begin, const T &end)
        {
            p_message out(new pitch_message(message_type::add_order));

            return out;
        }

        /**
         * decode and validate Order Executed message
         */
        template <typename T>
        static p_message decode_executed(const T &begin, const T &end)
        {
            p_message out(new pitch_message(message_type::order_executed));

            return out;
        }

        /**
         * decode and validate Order Cancel message
         */
        template <typename T>
        static p_message decode_cancel(const T &begin, const T &end)
        {
            p_message out(new pitch_message(message_type::order_cancel));

            return out;
        }

        /**
         * decode and validate Trade message
         */
        template <typename T>
        static p_message decode_trade(const T &begin, const T &end)
        {
            p_message out(new pitch_message(message_type::trade_message));

            return out;
        }
    };
}
#endif