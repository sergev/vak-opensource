# -*- coding: utf-8 -*-
'''
Сортировка пузырьком
'''


def sort_bubble(data):
    changed = True

    while changed:
        changed = False
        for n in range(1, len(data)):
            if data[n] < data[n - 1]:
                data[n], data[n - 1] = data[n - 1], data[n]
                changed = True
