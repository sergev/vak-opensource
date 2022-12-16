//
// Solutions of the Two-Sum task from:
// https://stackoverflow.com/questions/42917306/the-improved-solution-for-2-sum-algorithm
//
object Solution {
    //
    // A straightforward solution with O(n^2) complexity.
    //
    def twoSum(nums: Array[Int], target: Int): Array[Int] = {
        //
        // Pair each number with it's index.
        // For example: Array((2,0), (7,1), (11,2), (15,3))
        //
        val withIndex = nums.zipWithIndex

        //
        // Compute all the possible combinations of two elements from the array.
        //
        val comb = withIndex.combinations(2)

        //
        // Find an item which has a required sum of numbers.
        // The result is Option[Array[(Int, Int)]].
        //
        val find = comb.find(_.map(_(0)).sum == target)

        //
        // We've been told our input has a solution.
        // So extract it from the option.
        // We get something like: Array((2,0), (7,1))
        //
        val res = find.get

        //
        // Get indices from the result.
        // For example: Array(0, 1)
        //
        val idx = res.map(_(1))

        idx
    }

    //
    // More efficient solution: it runs in linear time.
    //
    def twoSum2(nums: Array[Int], target: Int): Array[Int] = {
        import scala.collection.immutable.HashMap

        def run(index: Int, map: HashMap[Int, Int]): Array[Int] = {
            val value = nums(index)
            map get (target - value) match {
                case Some(foundInd) => Array(foundInd, index)
                case None => run(index + 1, map + (value -> index))
            }
        }
        run(0, HashMap())
    }

    //
    // Trivial solution.
    //
    def twoSum3(nums: Array[Int], target: Int): Array[Int] = {
        for (j <- nums.indices) {
            for (i <- 0 until j) {
                if (nums(i) + nums(j) == target) {
                    return Array(i, j)
                }
            }
        }
        throw Exception("no solution")
    }
}
