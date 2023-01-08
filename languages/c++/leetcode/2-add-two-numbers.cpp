//
// You are given two non-empty linked lists representing two
// non-negative integers. The digits are stored in reverse order,
// and each of their nodes contains a single digit. Add the two
// numbers and return the sum as a linked list.
//
// You may assume the two numbers do not contain any leading zero,
// except the number 0 itself.
//
//using namespace std;

//
// Definition for singly-linked list.
//
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        ListNode *result = nullptr;
        ListNode **tail = &result;
        int a, b, carry = 0;
        for (;;) {
            if (l1 != nullptr) {
                a = l1->val;
                l1 = l1->next;
                if (l2 != nullptr) {
                    b = l2->val;
                    l2 = l2->next;
                } else {
                    b = 0;
                }
            } else {
                a = 0;
                if (l2 != nullptr) {
                    b = l2->val;
                    l2 = l2->next;
                } else {
                    b = 0;
                    if (carry == 0)
                        break;
                }
            }
            int sum = a + b + carry;
            *tail = new ListNode(sum % 10);
            tail = &(*tail)->next;
            carry = sum / 10;
        }
        return result;
    }
};

#include "gtest/gtest.h"

//
// Example 1:
// Input: l1 = [2,4,3], l2 = [5,6,4]
// Output: [7,0,8]
// Explanation: 342 + 465 = 807.
//
TEST(add_two_numbers, case_1)
{
    Solution test;
    auto l1 = new ListNode(2, new ListNode(4, new ListNode(3)));
    auto l2 = new ListNode(5, new ListNode(6, new ListNode(4)));
    auto result = test.addTwoNumbers(l1, l2);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->val, 7);
    ASSERT_NE(result->next, nullptr);
    EXPECT_EQ(result->next->val, 0);
    ASSERT_NE(result->next->next, nullptr);
    EXPECT_EQ(result->next->next->val, 8);
    EXPECT_EQ(result->next->next->next, nullptr);
}

//
// Example 2:
// Input: l1 = [0], l2 = [0]
// Output: [0]
//
TEST(add_two_numbers, case_2)
{
    Solution test;
    auto l1 = new ListNode(0);
    auto l2 = new ListNode(0);
    auto result = test.addTwoNumbers(l1, l2);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->val, 0);
    EXPECT_EQ(result->next, nullptr);
}

//
// Example 3:
// Input: l1 = [9,9,9,9,9,9,9], l2 = [9,9,9,9]
// Output: [8,9,9,9,0,0,0,1]
//
TEST(add_two_numbers, case_3)
{
    Solution test;
    auto l1 = new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9)))))));
    auto l2 = new ListNode(9, new ListNode(9, new ListNode(9, new ListNode(9))));
    auto result = test.addTwoNumbers(l1, l2);
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->val, 8);
    ASSERT_NE(result->next, nullptr);

    EXPECT_EQ(result->next->val, 9);
    ASSERT_NE(result->next->next, nullptr);

    EXPECT_EQ(result->next->next->val, 9);
    ASSERT_NE(result->next->next->next, nullptr);

    EXPECT_EQ(result->next->next->next->val, 9);
    ASSERT_NE(result->next->next->next->next, nullptr);

    EXPECT_EQ(result->next->next->next->next->val, 0);
    ASSERT_NE(result->next->next->next->next->next, nullptr);

    EXPECT_EQ(result->next->next->next->next->next->val, 0);
    ASSERT_NE(result->next->next->next->next->next->next, nullptr);

    EXPECT_EQ(result->next->next->next->next->next->next->val, 0);
    ASSERT_NE(result->next->next->next->next->next->next->next, nullptr);

    EXPECT_EQ(result->next->next->next->next->next->next->next->val, 1);
    EXPECT_EQ(result->next->next->next->next->next->next->next->next, nullptr);
}
