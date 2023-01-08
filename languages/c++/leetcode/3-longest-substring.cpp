//
// Given a string s, find the length of the longest substring
// without repeating characters.
//
#include <string>
#include <unordered_set>

using namespace std;

class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        unordered_set<char> substring;
        unsigned start = 0, end = 0, maxlen = 0;
        for (; end < s.size(); end++) {
            char c = s[end];
            if (substring.count(c) == 0) {
                // Add next char to the set, if it's not there.
                // Remember the length.
                substring.insert(c);
                if (maxlen < substring.size()) {
                    maxlen = substring.size();
                }
            } else {
                // Otherwise advance the start index removing characters from the set.
                for (;;) {
                    assert(start < end);
                    char x = s[start];
                    start++;
                    if (x == c) {
                        break;
                    }
                    substring.erase(x);
                }
            }
        }
        return (int)maxlen;
    }
};

#include "gtest/gtest.h"

//
// Example 1
// Input: s = "abcabcbb"
// Output: 3
// Explanation: The answer is "abc", with the length of 3.
//
TEST(longest_substring, case_1)
{
    Solution test;
    auto result = test.lengthOfLongestSubstring("abcabcbb");
    EXPECT_EQ(result, 3);
}

//
// Example 2
// Input: s = "bbbbb"
// Output: 1
// Explanation: The answer is "b", with the length of 1.
//
TEST(longest_substring, case_2)
{
    Solution test;
    auto result = test.lengthOfLongestSubstring("bbbbb");
    EXPECT_EQ(result, 1);
}

//
// Example 3
// Input: s = "pwwkew"
// Output: 3
// Explanation: The answer is "wke", with the length of 3.
// Notice that the answer must be a substring, "pwke" is a subsequence and not a substring.
//
TEST(longest_substring, case_3)
{
    Solution test;
    auto result = test.lengthOfLongestSubstring("pwwkew");
    EXPECT_EQ(result, 3);
}

TEST(longest_substring, space)
{
    Solution test;
    auto result = test.lengthOfLongestSubstring(" ");
    EXPECT_EQ(result, 1);
}
