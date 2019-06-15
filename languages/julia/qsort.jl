#
# Numeric vector sort
#
using Printf, Random

function qsort!(a, lo, hi)
    i, j = lo, hi
    while i < hi
        pivot = a[(lo + hi) >>> 1]
        while i <= j
            while a[i] < pivot
                i += 1
            end
            while a[j] > pivot
                j -= 1
            end
            if i <= j
                a[i], a[j] = a[j], a[i]
                i, j = i+1, j-1
            end
        end
        if lo < j
            qsort!(a, lo, j)
        end
        lo, j = i, hi
    end
    return a
end

# seed rng for more consistent timings
Random.seed!(1776)

n = 10000000
data = rand(n)
t0 = time_ns()
sorted = qsort!(data, 1, n)
t1 = time_ns()

@printf("size = %d, msec = %.1f\n", n, (t1 - t0) / 1e6)
