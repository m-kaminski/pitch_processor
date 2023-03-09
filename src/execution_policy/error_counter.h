#ifndef _ERROR_COUNTER_H
#define _ERROR_COUNTER_H

#include <iostream>

namespace pitchstream
{
    template <typename T>
    class error_counter
    {
    public:
        error_counter() : parse_error_counter(0),
                          processing_error_counter(0),
                          lines_skipped(0) {}

        bool has_errors()
        {
            return (parse_error_counter || processing_error_counter || lines_skipped);
        }
        void print_errors()
        {
            std::cerr << "Following errors were detected during execution:\n";

            if (parse_error_counter)
            {
                std::cerr << parse_error_counter << " parse errors detected and ignored" << std::endl;
            }
            if (processing_error_counter)
            {
                std::cerr << processing_error_counter << " processing errors detected and ignored" << std::endl;
            }
            if (lines_skipped)
            {
                std::cerr << lines_skipped << " input lines skipped" << std::endl;
            }
        }

        T parse_error_counter = 0;
        T processing_error_counter = 0;
        T lines_skipped = 0;
    };
}
#endif
