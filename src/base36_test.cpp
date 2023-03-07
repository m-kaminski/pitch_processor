#include "gtest/gtest.h"

#include "base36.h"
#include <iostream>
#include <string>
#include <cassert>

namespace pitchstream
{
    namespace
    {
        class base36_test : public ::testing::Test
        {
        };

        TEST_F(base36_test, conversion_1_MAX)
        {
            std::string B36_1("1");
            std::string B36_10("A");
            std::string B36_144("40");
            std::string B36_948437811("FOOBAR");
            std::string B36_MAX("ZZZZZZZZZZZZ");
            EXPECT_EQ(base36(B36_1), 1);
            EXPECT_EQ(base36(B36_10), 10);
            EXPECT_EQ(base36(B36_144), 144);
            EXPECT_EQ(base36(B36_948437811), 948437811);
            EXPECT_EQ(base36(B36_MAX), base36_max);
            EXPECT_EQ(B36_MAX.size(), 12);
        }

        TEST_F(base36_test, conversion_10_MAX_iterators)
        {
            std::string B36_10("A");
            std::string B36_144("40");
            std::string B36_948437811("FOOBAR");
            std::string B36_MAX("ZZZZZZZZZZZZ");
            EXPECT_EQ(base36(B36_10.begin(), B36_10.end()), 10);
            EXPECT_EQ(base36(B36_144.begin(), B36_144.end()), 144);
            EXPECT_EQ(base36(B36_948437811.begin(), B36_948437811.end()), 948437811);
            EXPECT_EQ(base36(B36_MAX.begin(), B36_MAX.end()), base36_max);
        }

        TEST_F(base36_test, conversion_0)
        {
            std::string B36_0("0");
            EXPECT_EQ(base36(B36_0.begin(), B36_0.end()), 0);
        }

        TEST_F(base36_test, conversion_empty)
        {
            std::string B36_E(""); // empty string converted to 0
            EXPECT_EQ(base36(B36_E.begin(), B36_E.end()), 0);
        }

        TEST_F(base36_test, conversion_too_long_throw)
        {
            std::string B36_L("1234567890123"); // 13 character
            EXPECT_THROW(base36(B36_L.begin(), B36_L.end()), std::invalid_argument);
            try
            {
                base36(B36_L.begin(), B36_L.end());
            }
            catch (const std::invalid_argument &e)
            {
                EXPECT_STREQ("Input string too long (12 digits at most)", e.what());
            }
        }

        TEST_F(base36_test, conversion_lowercase_throw)
        {
            std::string B36_L("foobar"); // Lowercase
            EXPECT_THROW(base36(B36_L.begin(), B36_L.end()), std::invalid_argument);
            try
            {
                base36(B36_L.begin(), B36_L.end());
            }
            catch (const std::invalid_argument &e)
            {
                EXPECT_STREQ("Not a base36 digit", e.what());
            }
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
