#ifndef _PITCH_FORMAT_CONSTANTS_H
#define _PITCH_FORMAT_CONSTANTS_H
namespace pitchstream
{
    // Codes for message types
    const char MESSAGE_ADD_ORDER = 'A';
    const char MESSAGE_ORDER_CANCEL = 'X';
    const char MESSAGE_ORDER_EXECUTED = 'E';
    const char MESSAGE_TRADE = 'P';

    // Offsets for all messages
    const int COMMON_ORDER_ID_OFFSET = 10;
    const int COMMON_ORDER_ID_LENGTH = 12;

    // Offsets for Order Add message
    const int ORDER_ADD_LENGTH = 46;
    const int ORDER_ADD_SHARES_OFFSET = 23;
    const int ORDER_ADD_SHARES_LENGTH = 6;
    const int ORDER_ADD_SYMBOL_OFFSET = 29;
    const int ORDER_ADD_SYMBOL_LENGTH = 6;

    // Offsets for Order Executed
    const int ORDER_EXECUTED_LENGTH = 40;
    const int ORDER_EXECUTED_SHARES_OFFSET = 22;
    const int ORDER_EXECUTED_SHARES_LENGTH = 6;

    // Offsets for Order Cancel message
    const int ORDER_CANCEL_LENGTH = 28;
    const int ORDER_CANCEL_SHARES_OFFSET = 23;
    const int ORDER_CANCEL_SHARES_LENGTH = 6;

    // Offsets for Order Trade message
    const int ORDER_TRADE_LENGTH = 57;
    const int ORDER_TRADE_SHARES_OFFSET = 23;
    const int ORDER_TRADE_SHARES_LENGTH = 6;
    const int ORDER_TRADE_SYMBOL_OFFSET = 29;
    const int ORDER_TRADE_SYMBOL_LENGTH = 6;

}
#endif
