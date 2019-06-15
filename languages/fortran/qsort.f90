!
! Numeric vector sort
!
program perf
    implicit none
    integer, parameter :: N = 10000000
    real*8 d(N)
    integer*8 t0, t1
    integer*8 hz

    call random_number(d)
    call system_clock(t0)
    call quicksort(d, 1, N)
    call system_clock(t1)

    call system_clock(count_rate=hz)
    print "('size = ', i0, ', msec = ', f0.1)", N, (t1 - t0) * 1000.0 / hz

contains

    recursive subroutine quicksort(a, lo0, hi)
        real*8 a(:)
        integer lo0, hi
        integer i, j, lo
        real*8 pivot, t

        lo = lo0
        i = lo
        j = hi
        do while (i < hi)
            pivot = a((lo+hi)/2)
            do while (i <= j)
                do while (a(i) < pivot)
                    i = i + 1
                end do
                do while (a(j) > pivot)
                    j = j - 1
                end do
                if (i <= j) then
                    t = a(i)
                    a(i) = a(j)
                    a(j) = t
                    i = i + 1
                    j = j - 1
                end if
            end do
            if (lo < j) call quicksort(a, lo, j)
            lo = i
            j = hi
        end do
    end subroutine

end program
