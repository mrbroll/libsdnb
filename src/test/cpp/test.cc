#include <cstring>
#include <iostream>
#include "../../sdnb/headers/gap_vector.hh"
#include "gtest/gtest.h"

using namespace std;
using namespace SDNB;

namespace {

    class GapBufferTest : public ::testing::Test
    {
        protected:
            GapBufferTest()
            {
                gb0 = new GapVector<char>();
                gb1 = new GapVector<char>();
                string testString = "this is a test string";
                gb0->insert(testString.begin(), testString.end());
                gb1->insert(testString.begin(), testString.end());
            }

            ~GapBufferTest()
            {
                delete gb0;
                delete gb1;
            }

            GapVector<char>* gb0;
            GapVector<char>* gb1;
    };

    TEST_F(GapBufferTest, InsertTest)
    {
        string testStr0 = "this is a test string";
        ASSERT_STREQ(testStr0.c_str(), string(gb0->begin(), gb0->end()).c_str());
        string testStr1 = ". adding more text here";
        gb0->insert(testStr1.begin(), testStr1.end());
        testStr1 = "this is a test string. adding more text here";
        ASSERT_STREQ(testStr1.c_str(), string(gb0->begin(), gb0->end()).c_str());
        string testStr2 = ".";
        gb1->insert(testStr2.begin(), testStr2.end());
        testStr2 = "this is a test string.";
        ASSERT_STREQ(testStr2.c_str(), string(gb1->begin(), gb1->end()).c_str());
    }

    TEST_F(GapBufferTest, RemoveTest)
    {
        gb0->remove(-6);
        string inStr0 = "dude.";
        gb0->insert(inStr0.begin(), inStr0.end());
        const char *testString0 = "this is a test dude.";
        ASSERT_STREQ(testString0, string(gb0->begin(), gb0->end()).c_str());
        gb1->remove(-1);
        string inStr1 = "g. And this is some more.";
        gb1->insert(inStr1.begin(), inStr1.end());
        const char *testString1 = "this is a test string. And this is some more.";
        ASSERT_STREQ(testString1, string(gb1->begin(), gb1->end()).c_str());
    }

    TEST_F(GapBufferTest, MoveGapTest)
    {
        gb0->moveGap(-6);
        string inStr0 = "mac daddy ";
        gb0->insert(inStr0.begin(), inStr0.end());
        const char *testString0 = "this is a test mac daddy string";
        ASSERT_STREQ(testString0, string(gb0->begin(), gb0->end()).c_str());
        gb0->moveGap(3);
        string inStr1 = "izz";
        gb0->insert(inStr1.begin(), inStr1.end());
        const char *testString1 = "this is a test mac daddy strizzing";
        ASSERT_STREQ(testString1, string(gb0->begin(), gb0->end()).c_str());
    }

    TEST_F(GapBufferTest, GrowShrinkTest)
    {
        string* testStr0 = new string("this is a test string");
        string* inStr0 = new string(1000, 'x');
        gb0->insert(inStr0->begin(), inStr0->end());
        //testStr0->append(*inStr0);
        ASSERT_STREQ(testStr0->c_str(), string(gb0->begin(), gb0->end()).c_str());
        delete testStr0;
        //delete inStr0;
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
