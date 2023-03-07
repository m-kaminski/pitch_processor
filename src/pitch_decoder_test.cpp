#include "gtest/gtest.h"

#include "pitch_message.h"
#include "pitch_decoder.h"
#include <string>
#include <memory>
#include <unordered_map>

namespace pitchstream
{
    namespace
    {
        class pitch_decode_test : public ::testing::Test
        {
        };
        

        const std::string   SAMPLE_ADD("S28800011AAK27GA0000DTS000100SH    0000619200Y");
        const std::string SAMPLE_CANCEL("S28800168X1K27GA00000Y000100");
        const std::string SAMPLE_TRADE("S28880387P4K27GA0000EZB000100DRYS  0001074900000A001NJEPR");
        const std::string SAMPLE_EXECUTE("S28800318E1K27GA00000X00010000001AQ00001");
        // convenient B36 conversion tool http://extraconversion.com/base-number/base-36

        TEST_F(pitch_decode_test, empty_string)
        {
            std::string E("");
            std::unique_ptr<pitch_message> p_m = pitch_decoder::decode(E.begin(), E.end());
            EXPECT_EQ(p_m.get(), nullptr);
        }

        TEST_F(pitch_decode_test, add_order_correct_type)
        {
            std::unique_ptr<pitch_message> p_m = 
                        pitch_decoder::decode(SAMPLE_ADD.begin(), SAMPLE_ADD.end());
            EXPECT_NE(p_m.get(), nullptr);
            EXPECT_EQ(p_m->get_type(), message_type::add_order);
            EXPECT_EQ(p_m->get_order_id(), 1389564350501069297LL);
            EXPECT_EQ(p_m->get_stock_symbol(), "SH");
            EXPECT_EQ(p_m->get_shares_count(), 100);
        }

        TEST_F(pitch_decode_test, add_order_short_line_fails)
        {
            std::string ADD_ORDER(SAMPLE_ADD);
            ADD_ORDER.pop_back();
            std::unique_ptr<pitch_message> p_m = 
                        pitch_decoder::decode(ADD_ORDER.begin(), ADD_ORDER.end());
            EXPECT_EQ(p_m.get(), nullptr);
        }

        TEST_F(pitch_decode_test, order_execute_correct_type)
        {
            std::unique_ptr<pitch_message> p_m = 
                        pitch_decoder::decode(SAMPLE_EXECUTE.begin(), SAMPLE_EXECUTE.end());
            EXPECT_NE(p_m.get(), nullptr);
            EXPECT_EQ(p_m->get_type(), message_type::order_executed);
        }

        TEST_F(pitch_decode_test, cancel_order_correct_type)
        {
            std::unique_ptr<pitch_message> p_m = 
                        pitch_decoder::decode(SAMPLE_CANCEL.begin(), SAMPLE_CANCEL.end());
            EXPECT_NE(p_m.get(), nullptr);
            EXPECT_EQ(p_m->get_type(), message_type::order_cancel);
            EXPECT_EQ(p_m->get_order_id(),  204969015920664610LL);
            EXPECT_EQ(p_m->get_shares_count(), 100);
        }

        TEST_F(pitch_decode_test, trade_message_correct_type)
        {
            std::unique_ptr<pitch_message> p_m = 
                        pitch_decoder::decode(SAMPLE_TRADE.begin(), SAMPLE_TRADE.end());
            EXPECT_NE(p_m.get(), nullptr);
            EXPECT_EQ(p_m->get_type(), message_type::trade_message);
        }


        // unordered map is intended structure for open orders
        TEST_F(pitch_decode_test, message_can_be_added_to_map) {
            std::unordered_map<int, std::unique_ptr<pitch_message> > mm;

            std::unique_ptr<pitch_message> p_m = 
                        pitch_decoder::decode(SAMPLE_CANCEL.begin(), SAMPLE_CANCEL.end());
            mm[1] = std::move(p_m);
            EXPECT_EQ(p_m.get(), nullptr);
            EXPECT_NE(mm.find(1), mm.end());
            EXPECT_EQ(mm.find(1)->second->get_type(), message_type::order_cancel);
        }

        TEST_F(pitch_decode_test, message_can_be_edited_on_map) {
            std::unordered_map<int, std::unique_ptr<pitch_message> > mm;
                        
            mm[1] = pitch_decoder::decode(SAMPLE_ADD.begin(), SAMPLE_ADD.end());
            EXPECT_NE(mm.find(1), mm.end());
            EXPECT_EQ(mm.find(1)->second->get_type(), message_type::add_order);
            EXPECT_EQ(mm.find(1)->second->get_shares_count(), 100);
            EXPECT_EQ(mm.find(1)->second->subtract_shares_count(20), 80);
            EXPECT_EQ(mm.find(1)->second->get_shares_count(), 80);
            mm.erase(1);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
