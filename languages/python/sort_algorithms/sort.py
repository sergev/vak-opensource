# -*- coding: utf-8 -*-

import array
import random
from datetime import datetime
from typing import Callable, MutableSequence

from bubble import sort_bubble
from qsort import qsort_middle, qsort_end


SIZE = 10000


def create_src_list(size):
    seed_init_value = 126

    random.seed(seed_init_value)
    data = [random.randint(0, 1000) for _ in range(size)]
    result = array.array('i', data)
    return result


def test_sort(data):
    for n in range(1, len(data)):
        assert data[n] >= data[n - 1]


def run_sort(func: Callable[[MutableSequence], None]) -> int:
    print('    Data creation...')
    data = create_src_list(SIZE)

    print('    Sorting...')
    start = datetime.now()
    func(data)
    end = datetime.now()
    delay = (end - start).microseconds

    print('    Test result...')
    test_sort(data)
    print('    Sorting time: {:.2f} ms'.format(delay / 1000))
    return delay


if __name__ == '__main__':
    print('Items count: {}\n'.format(SIZE))
    print('Bubble sort...')
    run_sort(sort_bubble)

    print('Quick sort (end)...')
    run_sort(qsort_end)

    print('Quick sort (middle)...')
    run_sort(qsort_middle)
