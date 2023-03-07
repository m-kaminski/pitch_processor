#include "gtest/gtest.h"

#include "pitch_message.h"
#include "pitch_decoder.h"
#include <string>
#include <memory>

namespace pitchstream
{
    namespace
    {
        class pitch_decode_test : public ::testing::Test
        {
        };
        
        TEST_F(pitch_decode_test, empty_string)
        {
            std::string E("");
            std::unique_ptr<pitch_message> p_m = pitch_decoder::decode(E.begin(), E.end());
            EXPECT_EQ(p_m.get(), nullptr);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
