# -*- coding: utf-8 -*-
'''
Быстрая сортировка
'''
from typing import MutableSequence, Callable


def _qsort(data: MutableSequence,
           start: int,
           end: int,
           process_func: Callable[[MutableSequence, int, int], int]) -> None:
    if start < end:
        pivot_index = process_func(data, start, end)
        _qsort(data, start, pivot_index - 1, process_func)
        _qsort(data, pivot_index + 1, end, process_func)


def _process_end(data: MutableSequence, start: int, end: int) -> int:
    i = start - 1
    j = end
    pivot = data[end]

    while i < j:
        i += 1
        j -= 1

        while data[i] < pivot:
            i += 1

        while data[j] > pivot and j > start:
            j -= 1

        if i < j:
            data[i], data[j] = data[j], data[i]

    data[i], data[end] = data[end], data[i]

    return i


def _process_middle(data: MutableSequence, start: int, end: int) -> int:
    i = start - 1
    j = end + 1

    pivot_index = (start + end) // 2
    pivot = data[pivot_index]

    while i < j:
        if i < pivot_index:
            i += 1
            while data[i] < pivot and i < pivot_index:
                i += 1

        if j > pivot_index:
            j -= 1
            while data[j] > pivot and j > pivot_index:
                j -= 1

        if i < j:
            data[i], data[j] = data[j], data[i]

            if i == pivot_index:
                pivot_index = j
            elif j == pivot_index:
                pivot_index = i

    return i


def qsort_end(data: MutableSequence):
    _qsort(data, 0, len(data) - 1, _process_end)


def qsort_middle(data: MutableSequence):
    _qsort(data, 0, len(data) - 1, _process_middle)
