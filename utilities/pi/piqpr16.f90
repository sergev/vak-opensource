program piqpr16

!   This program implements the BBP algorithm to generate a few hexadecimal
!   digits beginning immediately after a given position id, or in other words
!   beginning at position id + 1.  The maximum value of id is not known for
!   this code, but it is believed to be at least 10^14, depending on system.
!   Results for a given position id can be checked by repeating with id-1 or
!   id+1, and verifying that the hex digits perfectly overlap with an offset of
!   one, except possibly for a few trailing digits.  The resulting fractions
!   are typically accurate to at least 25 decimal digits, and to at least 20
!   hex digits.  This code runs reasonably fast on IBM systems and others that
!   treat "real*16" as double-double, but typically rather slowly on systems
!   that treat "real*16" as software-based IEEE 128-bit floating-point.

!   David H. Bailey     2006-09-06

implicit none
integer id, nbt
real tm0, tm1
real*16 c1, pid, s1, s2, s3, s4, series
parameter (nbt = 96)
integer ibt(nbt)
character*1 chx(nbt/4)

!  Set the position id here.  Output hex digits beginning after position id,
!  i.e., beginning with position id + 1.

parameter (id = 1000000)

write (6, 1) id
1 format ('Pi hex digit computation'/'position =',i12)

!   Evaluate the four series used in the formula.

call cpu_time (tm0)
s1 = series (1, id)
call ibits (s1, nbt, ibt, chx)
write (6, 2) 1, s1, chx, ibt
2 format ('Sum of series',i2,' ='/f34.30/24a1/96i1.1)
s2 = series (4, id)
call ibits (s2, nbt, ibt, chx)
write (6, 2) 2, s2, chx, ibt
s3 = series (5, id)
call ibits (s3, nbt, ibt, chx)
write (6, 2) 3, s3, chx, ibt
s4 = series (6, id)
call ibits (s4, nbt, ibt, chx)
write (6, 2) 4, s4, chx, ibt

!   Compute the four series results as specified in the BBP formula.

c1 = 1.d0
pid = mod (4.d0 * s1 - 2.d0 * s2 - s3 - s4, c1)
if (pid .lt. 0.d0) pid = pid + 1.d0
call cpu_time (tm1)

!   Output results.

write (6, 3) tm1 - tm0
3 format ('Run complete.  CPU time =',f12.4)
call ibits (pid, nbt, ibt, chx)
write (6, 4) pid, chx, ibt
4 format ('Pi fraction ='/f34.30/24a1/96i1.1)

stop
end

subroutine ibits (x, nbt, ibt, chx)

!   This returns in ibt the first nbt bits of the fraction of the floating-point
!   argument x, and returns the first nbt/4 hex digits in chx.  The argument nbt
!   must be divisible by 4 and must not exceed the number of mantissa bits in
!   the floating-point format used for x.

implicit none
integer i, ii, nbt, ibt(nbt)
real*16 x, y
character*1 chx(nbt/4), hx(0:15)
data hx/'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', &
   'C', 'D', 'E', 'F'/

y = abs (x)

do i = 1, nbt
  y = 2.d0 * (y - aint (y))
  ibt(i) = int (y)
enddo

do i = 1, nbt / 4
  ii = 8 * ibt(4*i-3) + 4 * ibt(4*i-2) + 2 * ibt(4*i-1) + ibt(4*i)
  chx(i) = hx(ii)
enddo

return
end

function series (m, id)

!   This routine evaluates the series Sum_{k=1}^d (2^(id-k) mod (8*k+m))/(8*k+m).

implicit none
integer id, k, m
real*16 ak, c1, eps, expm1, p, s, series, t
parameter (eps = 1d-32)

c1 = 1.d0
s = 0.d0

!  Sum the series until k exceeds id

do k = 0, id
  ak = 8 * k + m
  p = id - k
  if (p == 0.d0) then
    t = 1.d0
  elseif (ak == 1.d0) then
    t = 0.d0
  else
    t = expm1 (p, ak)
  endif
  s = mod (s + t / ak, c1)
!  write (6, '(i5,3f15.1,f20.15)') k, p, ak, t, s
enddo

!   Complete the computation by manually computing a few terms where k > id.

do k = id + 1, id + 100
  ak = 8 * k + m
  t = 16.d0 ** (id - k)
  if (t .lt. eps) goto 120
  s = mod (s + t / ak, c1)
enddo

120 continue

! write (6, *) 'final s =', s

series = s
return
end

function expm1 (p, ak)

!   expm1 (p, ak) = 16^p mod ak.
!   This routine uses a left-to-right binary exponentiation scheme.

implicit none
integer i, ntp
parameter (ntp = 30)
real*16 ak, expm1, p, p1, pt1, r, tp(ntp)
save tp
data tp/ntp*0.d0/

!   If this is the first call, fill the power of two table tp.

if (tp(1) .eq. 0.d0) then
  tp(1) = 1.d0

  do i = 2, ntp
    tp(i) = 2.d0 * tp(i-1)
  enddo
endif

!   Find the largest power of two less than or equal to p.

do i = 1, ntp
  if (tp(i) .gt. p) goto 100
enddo

100 continue

pt1 = tp(i-1)
p1 = p
r = 1.d0

!  Perform the binary exponentiation scheme.

110 continue

if (p1 .ge. pt1) then
  r = mod (16.d0 * r, ak)
  p1 = p1 - pt1
endif
pt1 = 0.5d0 * pt1
if (pt1 .ge. 1.d0) then
  r = mod (r * r, ak)
  goto 110
endif

expm1 = mod (r, ak)

return
end
