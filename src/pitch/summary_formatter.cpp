#include <iomanip>
#include <algorithm>

#include "summary_fotmatter.h"


namespace pitchstream
{

    void format_summary(std::ostream & output, const event_accumulator::v_summary & input) {
        if (input.empty())
            return;
        uint64_t largest_count = std::max_element(input.begin(), input.end(),
                                 [] (auto & p1, auto & p2){return p1.second < p2.second;})->second;
        
        int longest_name = std::max_element(input.begin(), input.end(),
                                 [] (auto & p1, auto & p2){return p1.first.size() < p2.first.size();})->first.size();

        int longest_count = 1;
        while (largest_count/=10) longest_count++;

        for (auto & p : input) {
            output << std::setw(longest_name) << std::left << p.first << "  " 
                    << std::setw(longest_count) << std::right << p.second << std::endl;
        }

    }

}