import org.junit.Test
import org.junit.Assert._

class TwoSum {

    //
    // Example 1:
    //
    //       Input: nums = [2,7,11,15], target = 9
    //      Output: [0,1]
    // Explanation: Because nums[0] + nums[1] == 9, we return [0, 1].
    //
    @Test def Example_1(): Unit = {
        val result = Solution.twoSum(Array(2, 7, 11, 15), 9)

        assertEquals(2, result.length);
        assertEquals(0, result(0));
        assertEquals(1, result(1));
    }

    //
    // Example 2:
    //
    //  Input: nums = [3,2,4], target = 6
    // Output: [1,2]
    //
    @Test def Example_2(): Unit = {
        val result = Solution.twoSum(Array(3, 2, 4), 6)

        assertEquals(2, result.length);
        assertEquals(1, result(0));
        assertEquals(2, result(1));
    }

    //
    // Example 3:
    //
    //  Input: nums = [3,3], target = 6
    // Output: [0,1]
    //
    @Test def Example_3(): Unit = {
        val result = Solution.twoSum(Array(3, 3), 6)

        assertEquals(2, result.length);
        assertEquals(0, result(0));
        assertEquals(1, result(1));
    }
}
