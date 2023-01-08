//
// Given an array of integers nums and an integer target, return
// indices of the two numbers such that they add up to target.
//
// You may assume that each input would have exactly one solution,
// and you may not use the same element twice.
//
// You can return the answer in any order.
//
#include <vector>
#include <unordered_map>

using namespace std;

class Solution {
public:
    //
    // Trivial solution: it runs in O(n^2) time.
    //
    vector<int> twoSum(vector<int>& nums, int target) {
        for (int j = 0; j < (int)nums.size(); j++) {
            for (int i = 0; i < j; i++) {
                if (nums[i] + nums[j] == target) {
                    return vector<int>{ i, j };
                }
            }
        }
        throw runtime_error("no solution");
    }

    //
    // More efficient solution: it runs in linear time.
    //
    vector<int> twoSum2(vector<int>& nums, int target) {
        unordered_map<int, int> complements;
        for (int j = 0; j < (int)nums.size(); j++) {
            auto num = nums[j];
            if (complements.count(num) > 0) {
                return vector<int>{ complements[num], j };
            }
            complements[target - num] = j;
        }
        throw runtime_error("no solution");
    }
};

#include "gtest/gtest.h"

//
// Example 1:
//
//       Input: nums = [2,7,11,15], target = 9
//      Output: [0,1]
// Explanation: Because nums[0] + nums[1] == 9, we return [0, 1].
//
TEST(two_sum, case_1)
{
    Solution test;
    vector<int> nums = { 2, 7, 11, 15 };
    auto result = test.twoSum(nums, 9);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 1);
}

TEST(two_sum_2, case_1)
{
    Solution test;
    vector<int> nums = { 2, 7, 11, 15 };
    auto result = test.twoSum2(nums, 9);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 1);
}

//
// Example 2:
//
//  Input: nums = [3,2,4], target = 6
// Output: [1,2]
//
TEST(two_sum, case_2)
{
    Solution test;
    vector<int> nums = { 3, 2, 4 };
    auto result = test.twoSum(nums, 6);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
}

TEST(two_sum_2, case_2)
{
    Solution test;
    vector<int> nums = { 3, 2, 4 };
    auto result = test.twoSum2(nums, 6);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 1);
    EXPECT_EQ(result[1], 2);
}

//
// Example 3:
//
//  Input: nums = [3,3], target = 6
// Output: [0,1]
//
TEST(two_sum, case_3)
{
    Solution test;
    vector<int> nums = { 3, 3 };
    auto result = test.twoSum(nums, 6);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 1);
}

TEST(two_sum_2, case_3)
{
    Solution test;
    vector<int> nums = { 3, 3 };
    auto result = test.twoSum2(nums, 6);
    EXPECT_EQ(result.size(), 2);
    EXPECT_EQ(result[0], 0);
    EXPECT_EQ(result[1], 1);
}
