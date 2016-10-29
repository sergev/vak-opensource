#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NTIMES  2       // Repeat test several times
#define NELEM   16      // Number of elements in data array, power of 2

// This array is initialized to put it into DATA section.
int arr_cached [NELEM] = { 1 };

// This array is not initialized to put it into BSS section.
int arr_uncached [NELEM];

void dump (int *array, int elements)
{
    int start, i;

    if (array >= arr_cached && array < arr_cached+NELEM) {
        start = array - arr_cached;
        printf ("cached:");
    } else {
        start = array - arr_uncached;
        printf ("uncached:");
    }

    for (i = 0; i < elements; i++)
        printf (" %d:%d", start + i, array[i]);
    printf ("\n");
}

// Public-domain implementation by Eugene Jitomirsky.
// Should be called with 2 elements and more.
void quickSort (int *array, int elements)
{
    dump (array, elements);

    int pivot = array[0];
    int left = 1;
    int right = elements-1;

    for (;;) {
        while (array[left] <= pivot && left < right)
            left++;
        while (array[right] > pivot && right >= left)
            right--;
        if (left >= right)
            break;

        int elem = array[left];
        array[left] = array[right];
        array[right] = elem;
    }
    array[0] = array[right];
    array[right] = pivot;

    if (right > 1)
        quickSort (array, right);

    if (right < elements-2)
        quickSort (&array[right+1], elements-right-1);
}

// Stop with the FAIL result
void failure ()
{
    printf ("FAIL\n");
    exit (1);
}

// Initialize, sort and check an array
void test_array (int *arr, int seed)
{
    int i;

    // Make some pseudo-random data
    for (i = 0; i < NELEM; i++)
        arr [i] = (i ^ seed ^ NELEM/2) & (NELEM-1);

    quickSort (arr, NELEM);
    dump (arr, NELEM);

    for (i = 0; i < NELEM; i++) {
        if (arr [i] != i)
            failure ();
    }
}

// Main test routine
int main ()
{
    int n;

    for (n = 0; n < NTIMES; n++) {
        test_array (arr_cached, n+1);
        test_array (arr_uncached, n+1);
    }

    // Stop with the PASS result
    printf ("PASS\n");
    return 0;
}
