#ifndef _PITCH_MESSAGE_H
#define _PITCH_MESSAGE_H
namespace pitchstream
{
    enum message_type
    {
        add_order,
        order_cancel,
        order_executed,
        trade_message
    };

    struct pitch_message
    {
        message_type type;
    };
}
#endif