program quine
character(1), parameter :: quote = achar(34)
character(*), parameter :: s(*) = [character(len=80) :: &
    "program quine", &
    "character(1), parameter :: quote = achar(34)", &
    "character(*), parameter :: s(*) = [character(len=80) :: &", &
    "    '']", &
    "integer :: i", &
    "do i = 1, 3", &
    "    print '(a)', trim(s(i))", &
    "end do", &
    "do i = 1, size(s)-1", &
    "    print '(5a)', '    ', quote, trim(s(i)), quote, ', &'", &
    "end do", &
    "do i = 4, size(s)-1", &
    "    print '(a)', trim(s(i))", &
    "end do", &
    "end program", &
    '']
integer :: i
do i = 1, 3
    print '(a)', trim(s(i))
end do
do i = 1, size(s)-1
    print '(5a)', '    ', quote, trim(s(i)), quote, ', &'
end do
do i = 4, size(s)-1
    print '(a)', trim(s(i))
end do
end program
