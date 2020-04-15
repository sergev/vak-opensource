#include "gtest/gtest.h"
#include "StringUtil.h"

using namespace std;
using namespace testing;
using namespace stringutil;

TEST(AString, AnswersHeadAsItsFirstLetter) {
   ASSERT_EQ(head("xyz"), "x");
}

TEST(AString, AnswersHeadAsEmptyWhenEmpty) {
   ASSERT_EQ(head(""), "");
}

TEST(AString, AnswersTailAsAllLettersAfterHead) {
   ASSERT_EQ(tail("xyz"), "yz");
}

TEST(AString, AnswersTailAsEmptyWhenEmpty) {
   ASSERT_EQ(tail(""), "");
}

TEST(AString, AnswersTailAsEmptyWhenContainsOnlyOneCharacter) {
   ASSERT_EQ(tail("X"), "");
}
