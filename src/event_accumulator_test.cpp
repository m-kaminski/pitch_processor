#include "gtest/gtest.h"

#include "pitch_message.h"
#include "event_accumulator.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace pitchstream
{
    namespace
    {
        class event_accumulator_test : public ::testing::Test
        {
        };

        TEST_F(event_accumulator_test, null_pointer_ignored)
        {
            event_accumulator a;
            std::unique_ptr<pitch_message> p_m;
            a.process_message(move(p_m));
            EXPECT_TRUE(a.generate_summary_all().empty());
        }

    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
