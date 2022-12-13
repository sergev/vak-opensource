//
// Trivial solution: it runs in O(n^2) time.
//
fn two_sum(nums: Vec<i32>, target: i32) -> Vec<i32> {
    for (i, a) in nums.iter().enumerate() {
        for (j, b) in nums[i+1..].iter().enumerate() {
            if a + b == target {
                return vec![i as i32, (j + i + 1) as i32];
            }
        }
    }
    panic!("no solution");
}

//
// Functional approach: also O(n^2) time.
//
fn two_sum_2(nums: Vec<i32>, target: i32) -> Vec<i32> {
    //
    // Pair each number with it's index.
    // For example: [(2, 0), (7, 1), (11, 2), (15, 3)]
    //
    let with_index: Vec<_> = nums.iter().zip(0..).collect();

    //
    // Compute all the possible combinations of two elements from the array.
    // Like this: [[(2, 0), (7, 1)], [(2, 0), (11, 2)], [(2, 0), (15, 3)],
    //             [(7, 1), (11, 2)], [(7, 1), (15, 3)], [(11, 2), (15, 3)]]
    //
    let comb = combination::combine::from_vec_at(&with_index, 2);

    //
    // Find an item which has a required sum of numbers.
    // The result is Some([(2, 0), (7, 1)]).
    //
    let find = comb.iter().find(|&x| x.iter().map(|&item| item.0).sum::<i32>() == target);

    //
    // We've been told our input has a solution.
    // So extract it from the option.
    // We get something like: [(2, 0), (7, 1)]
    //
    let res = find.unwrap();

    //
    // Get indices from the result.
    // For example: [0, 1]
    //
    let idx: Vec<_> = res.iter().map(|&x| x.1).collect();

    idx
}

//
// More efficient solution: it runs in linear time.
//
fn two_sum_3(nums: Vec<i32>, target: i32) -> Vec<i32> {
    use std::collections::HashMap;
    let mut complements: HashMap<i32, i32> = HashMap::new();

    for (i, num) in nums.iter().enumerate() {
        if let Some(&index) = complements.get(num) {
            return vec![index, i as i32];
        }
        complements.insert(target - num, i as i32);
    }
    panic!("no solution");
}

#[cfg(test)]

//
// Example 1:
//
//       Input: nums = [2,7,11,15], target = 9
//      Output: [0,1]
// Explanation: Because nums[0] + nums[1] == 9, we return [0, 1].
//
#[test]
fn example_1() {
    let result = two_sum(vec![2, 7, 11, 15], 9);
    assert_eq!(result.len(), 2);
    assert_eq!(result[0], 0);
    assert_eq!(result[1], 1);
}

//
// Example 2:
//
//  Input: nums = [3,2,4], target = 6
// Output: [1,2]
//
#[test]
fn example_2() {
    let result = two_sum(vec![3, 2, 4], 6);
    assert_eq!(result.len(), 2);
    assert_eq!(result[0], 1);
    assert_eq!(result[1], 2);
}

//
// Example 3:
//
//  Input: nums = [3,3], target = 6
// Output: [0,1]
//
#[test]
fn example_3() {
    let result = two_sum(vec![3, 3], 6);
    assert_eq!(result.len(), 2);
    assert_eq!(result[0], 0);
    assert_eq!(result[1], 1);
}
