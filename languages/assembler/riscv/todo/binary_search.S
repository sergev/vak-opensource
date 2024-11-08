// int binsearch(const int arr[], int needle, int size) {
//     int mid;
//     int left = 0;
//     int right = size -1;
//
//     while (left <= right) {
//         mid = (left + right) / 2;
//         // Needle is bigger than what we're looking at,
//         // only consider the upper half of the list.
//         if (needle > arr[mid]) {
//             left = mid + 1;
//         }
//         else if (needle < arr[mid]) {
//             right = mid - 1;
//         }
//         else {
//             return mid;
//         }
//     }
//     // We've gone through all elements and the needle wasn't found.
//     return -1;
// }
.section .text
.global binsearch
binsearch:
    # a0 = int arr[]
    # a1 = int needle
    # a2 = int size
    # t0 = mid
    # t1 = left
    # t2 = right

    li      t1, 0        # left = 0
    addi    t2, a2, -1   # right = size - 1
1: # while loop
    bgt     t1, t2, 1f   # left > right, break
    add     t0, t1, t2   # mid = left + right
    srai    t0, t0, 1    # mid = (left + right) / 2

    # Get the element at the midpoint
    slli    t4, t0, 2    # Scale the midpoint by 4
    add     t4, a0, t4   # Get the memory address of arr[mid]
    lw      t4, 0(t4)    # Dereference arr[mid]

    # See if the needle (a1) > arr[mid] (t3)
    ble     a1, t4, 2f   # if needle <= t3, we need to check the next condition
    # If we get here, then the needle is > arr[mid]
    addi    t1, t0, 1    # left = mid + 1
    j       1b
2:
    bge     a1, t4, 2f   # skip if needle >= arr[mid]
    # If we get here, then needle < arr[mid]
    addi    t2, t0, -1   # right = mid - 1
    j       1b
2:
    # If we get here, then needle == arr[mid]
    mv      a0, t0
1:
    ret
