#ifndef _PITCH_MESSAGE_H
#define _PITCH_MESSAGE_H
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
        message_type get_type() {return type;};

        private:
        pitch_message() {}
        pitch_message(message_type t) : type(t) {}
        message_type type;
        friend class pitch_decoder;
    };
}
#endif