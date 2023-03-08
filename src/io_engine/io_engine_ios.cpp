#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>


#include <iterator>

#include "io_engine.h"
#include "io_engine_ios.h"

namespace pitchstream
{

    void io_engine_ios::process_input(io_engine::line_handler handler)
    {
        std::ios_base::sync_with_stdio(false);
        std::cin.tie(NULL);
        std::string s;
        while (std::getline(std::cin, s))
        {
            // for each event
            handler(&*s.begin(), &*(s.begin() + s.size()) );
        }
    }

}
