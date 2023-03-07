#include "gtest/gtest.h"

#include <iomanip>
#include <algorithm>
#include <sstream>
#include "summary_fotmatter.h"

namespace pitchstream
{
    namespace
    {
        class summary_fotmatter_test : public ::testing::Test
        {
        };
        using v_summary_item = event_accumulator::v_summary_item;

        TEST_F(summary_fotmatter_test, empty_input)
        {
            std::stringstream ss;

            pitchstream::event_accumulator::v_summary vs;
            format_summary(ss, vs);
            EXPECT_TRUE(ss.str().empty());
        }

        TEST_F(summary_fotmatter_test, basic_formatting)
        {
            std::stringstream ss;

            pitchstream::event_accumulator::v_summary vs;
            vs.push_back(v_summary_item("AAPL", 90809));
            vs.push_back(v_summary_item("F", 1345135135));
            vs.push_back(v_summary_item("AAAAA", 4));
            vs.push_back(v_summary_item("N", 434));
            format_summary(ss, vs);
            EXPECT_EQ(ss.str(), std::string(
                                    "AAPL        90809\n"
                                    "F      1345135135\n"
                                    "AAAAA           4\n"
                                    "N             434\n"));
        }

        TEST_F(summary_fotmatter_test, single_line)
        {
            std::stringstream ss;

            pitchstream::event_accumulator::v_summary vs;
            vs.push_back(v_summary_item("N", 434));
            format_summary(ss, vs);
            EXPECT_EQ(ss.str(), std::string("N  434\n"));
        }

        TEST_F(summary_fotmatter_test, basic_formatting_alignment_depends_on_width)
        {
            std::stringstream ss;

            pitchstream::event_accumulator::v_summary vs;
            vs.push_back(v_summary_item("AAPL", 90809));
            vs.push_back(v_summary_item("F", 45135135));
            vs.push_back(v_summary_item("AAAAA", 4));
            vs.push_back(v_summary_item("N", 434));
            format_summary(ss, vs);
            EXPECT_EQ(ss.str(), std::string(
                                    "AAPL      90809\n"
                                    "F      45135135\n"
                                    "AAAAA         4\n"
                                    "N           434\n"));
        }

        TEST_F(summary_fotmatter_test, testcase_from_instruction)
        {
            std::stringstream ss;

            pitchstream::event_accumulator::v_summary vs;
            vs.push_back(v_summary_item("SPY", 24486275));
            vs.push_back(v_summary_item("QQQQ", 15996041));
            vs.push_back(v_summary_item("XLF", 10947444));
            vs.push_back(v_summary_item("IWM", 9362518));
            vs.push_back(v_summary_item("MSFT", 8499146));
            vs.push_back(v_summary_item("DUG", 8220682));
            vs.push_back(v_summary_item("C", 6756932));
            vs.push_back(v_summary_item("F", 6679883));
            vs.push_back(v_summary_item("EDS", 6673983));
            vs.push_back(v_summary_item("QID", 6526201));
            format_summary(ss, vs);
            EXPECT_EQ(ss.str(), std::string(
                                    "SPY   24486275\n"
                                    "QQQQ  15996041\n"
                                    "XLF   10947444\n"
                                    "IWM    9362518\n"
                                    "MSFT   8499146\n"
                                    "DUG    8220682\n"
                                    "C      6756932\n"
                                    "F      6679883\n"
                                    "EDS    6673983\n"
                                    "QID    6526201\n"));
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
