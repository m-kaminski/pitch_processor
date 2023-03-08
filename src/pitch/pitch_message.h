#ifndef _PITCH_MESSAGE_H
#define _PITCH_MESSAGE_H
#include <cstdint>
#include <string>
namespace pitchstream
{
    class pitch_decoder;

    enum message_type
    {
        add_order,
        order_cancel,
        order_executed,
        trade_message
    };

    /***
     * Class representing single message, it is constructed in pitch_decoder, which
     * implements factory for this class
     */
    class pitch_message
    {
        public:
        // type of a message
        message_type get_type() {return type;}

        uint64_t get_order_id() {return order_id;}

        uint32_t get_shares_count() {return shares_count;}

        /**
         * subtract from shares count, return new count
         */
        uint32_t subtract_shares_count(uint32_t count) {return shares_count -= count;}

        const std::string & get_stock_symbol() {return stock_symbol;}

        private:
        // private constructors - don't allow constructing outside factory (or tests)
        pitch_message() {}
        pitch_message(message_type t) : type(t) {}

        // internal data
        message_type type;
        uint64_t order_id;
        uint32_t shares_count;
        std::string stock_symbol;
        // actual factory class
        friend class pitch_decoder;
        // helper functions for tests
        friend pitch_message * generate_add(uint64_t order_id, uint32_t shares_count, const std::string & stock_symbol);
        friend pitch_message * generate_execute(uint64_t order_id, uint32_t shares_count);
        friend pitch_message * generate_cancel(uint64_t order_id, uint32_t shares_count);
        friend pitch_message * generate_trade(uint64_t order_id, uint32_t shares_count, const std::string & stock_symbol);
    };
}
#endif