#ifndef _PITCH_DECODER_H
#define _PITCH_DECODER_H

#include <iostream>
#include "event_accumulator.h"

namespace pitchstream
{

    void format_summary(std::ostream & output, const event_accumulator::v_summary & input);

}
#endif