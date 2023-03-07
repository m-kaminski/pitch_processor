#ifndef _PITCH_DECODER_H
#define _PITCH_DECODER_H
#include <memory>
#include <algorithm>
#include "base36.h"
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
            case MESSAGE_ADD_ORDER:
                return decode_add(begin, end); // add
            case MESSAGE_ORDER_CANCEL:
                return decode_cancel(begin, end); // cancel
            case MESSAGE_ORDER_EXECUTED:
                return decode_executed(begin, end); // executed
            case MESSAGE_TRADE:
                return decode_trade(begin, end); // trade
            }
            return p_message();
        }

    private:
        static const int COMMON_ORDER_ID_OFFSET = 10;
        static const int COMMON_ORDER_ID_LENGTH = 12;

        /**
         * decode and validate Add Order message
         */
        template <typename T>
        static p_message decode_add(const T &begin, const T &end)
        {
            const int ORDER_ADD_LENGTH = 46;
            const int ORDER_ADD_SHARES_OFFSET = 23;
            const int ORDER_ADD_SHARES_LENGTH = 6;
            const int ORDER_ADD_SYMBOL_OFFSET = 29;
            const int ORDER_ADD_SYMBOL_LENGTH = 6;

            if (end - begin < ORDER_ADD_LENGTH)
            {
                return p_message();
            }
            p_message out(new pitch_message(message_type::add_order));

            out->order_id = base36(begin + COMMON_ORDER_ID_OFFSET,
                                   begin + COMMON_ORDER_ID_OFFSET + COMMON_ORDER_ID_LENGTH);

            out->shares_count = std::stoi(std::string(
                begin + ORDER_ADD_SHARES_OFFSET,
                begin + ORDER_ADD_SHARES_OFFSET + ORDER_ADD_SHARES_LENGTH));

            T symbol_b = begin + ORDER_ADD_SYMBOL_OFFSET;
            T symbol_e = symbol_b + ORDER_ADD_SYMBOL_LENGTH;
            symbol_e = find(symbol_b, symbol_e, ' ');
            out->stock_symbol = std::string(symbol_b, symbol_e);

            return out;
        }

        /**
         * decode and validate Order Executed message
         */
        template <typename T>
        static p_message decode_executed(const T &begin, const T &end)
        {
            const int ORDER_EXECUTED_LENGTH = 40;
            const int ORDER_EXECUTED_SHARES_OFFSET = 22;
            const int ORDER_EXECUTED_SHARES_LENGTH = 6;

            if (end - begin < ORDER_EXECUTED_LENGTH)
            {
                return p_message();
            }

            p_message out(new pitch_message(message_type::order_executed));

            out->order_id = base36(begin + COMMON_ORDER_ID_OFFSET,
                                   begin + COMMON_ORDER_ID_OFFSET + COMMON_ORDER_ID_LENGTH);

            out->shares_count = std::stoi(std::string(
                begin + ORDER_EXECUTED_SHARES_OFFSET,
                begin + ORDER_EXECUTED_SHARES_OFFSET + ORDER_EXECUTED_SHARES_LENGTH));
            return out;
        }

        /**
         * decode and validate Order Cancel message
         */
        template <typename T>
        static p_message decode_cancel(const T &begin, const T &end)
        {
            const int ORDER_CANCEL_LENGTH = 28;
            const int ORDER_CANCEL_SHARES_OFFSET = 23;
            const int ORDER_CANCEL_SHARES_LENGTH = 6;

            if (end - begin < ORDER_CANCEL_LENGTH)
            {
                return p_message();
            }

            p_message out(new pitch_message(message_type::order_cancel));

            out->order_id = base36(begin + COMMON_ORDER_ID_OFFSET,
                                   begin + COMMON_ORDER_ID_OFFSET + COMMON_ORDER_ID_LENGTH);

            out->shares_count = std::stoi(std::string(
                begin + ORDER_CANCEL_SHARES_OFFSET,
                begin + ORDER_CANCEL_SHARES_OFFSET + ORDER_CANCEL_SHARES_LENGTH));

            return out;
        }

        /**
         * decode and validate Trade message
         */
        template <typename T>
        static p_message decode_trade(const T &begin, const T &end)
        {
            const int ORDER_TRADE_LENGTH = 57;
            const int ORDER_TRADE_SHARES_OFFSET = 23;
            const int ORDER_TRADE_SHARES_LENGTH = 6;

            const int ORDER_TRADE_SYMBOL_OFFSET = 29;
            const int ORDER_TRADE_SYMBOL_LENGTH = 6;

            if (end - begin < ORDER_TRADE_LENGTH)
            {
                return p_message();
            }

            p_message out(new pitch_message(message_type::trade_message));

            out->shares_count = std::stoi(std::string(
                begin + ORDER_TRADE_SHARES_OFFSET,
                begin + ORDER_TRADE_SHARES_OFFSET + ORDER_TRADE_SHARES_LENGTH));

            auto symbol_b = begin + ORDER_TRADE_SYMBOL_OFFSET;
            auto symbol_e = symbol_b + ORDER_TRADE_SYMBOL_LENGTH;
            symbol_e = find(symbol_b, symbol_e, ' ');
            out->stock_symbol = std::string(symbol_b, symbol_e);

            return out;
        }
    };
}
#endif