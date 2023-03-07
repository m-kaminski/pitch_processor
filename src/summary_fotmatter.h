#ifndef _SUMMARY_FORMATTER_H
#define _SUMMARY_FORMATTER_H

#include <iostream>
#include "event_accumulator.h"

namespace pitchstream
{
    /**
     * Format summary in human-readable form
     * @param input vector of sorted symbols/shares
     * @param output output stream
     */
    void format_summary(std::ostream & output, const event_accumulator::v_summary & input);

}
#endif