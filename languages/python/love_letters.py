#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Love Letters
# copyright (c) 2014, 2024 Nick Montfort <nickm@nickm.com>
# based on a program by Christopher Strachey, 1953
# intended to work in Python 2 (>= 2.5) as well as 3, however:
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved. This file is offered as-is,
# without any warranty.
#
# Updated 10 March 2015 to correct the word lists. Sources are photographs
# of Christoper Stachey’s notes in the Bodleian Library from J.R. Carpenter
# and table 14.1 in Noah Wardrip-Fruin’s article “Digital Media Archaeology.”
# Updated 31 May 2018 for compatibility with Python 2 (2.5+) and 3.
# Updated 1 January 2024 for Memory Slam 2.0.

from sys import argv
from random import choice
import textwrap
from time import sleep

first = ['Darling', 'Dear', 'Honey', 'Jewel']
second = ['duck', 'love', 'moppet', 'sweetheart']
adjectives = [
    'adorable',
    'affectionate',
    'amorous',
    'anxious',
    'ardent',
    'avid',
    'breathless',
    'burning',
    'covetous',
    'craving',
    'curious',
    'darling',
    'dear',
    'devoted',
    'eager',
    'erotic',
    'fervent',
    'fond',
    'impatient',
    'keen',
    'little',
    'loveable',
    'lovesick',
    'loving',
    'passionate',
    'precious',
    'sweet',
    'sympathetic',
    'tender',
    'unsatisfied',
    'wistful',
]
nouns = [
    'adoration',
    'affection',
    'ambition',
    'appetite',
    'ardour',
    'charm',
    'desire',
    'devotion',
    'eagerness',
    'enchantment',
    'enthusiasm',
    'fancy',
    'fellow feeling',
    'fervour',
    'fondness',
    'heart',
    'hunger',
    'infatuation',
    'liking',
    'longing',
    'love',
    'lust',
    'passion',
    'rapture',
    'sympathy',
    'tenderness',
    'thirst',
    'wish',
    'yearning',
]
adverbs = [
    'affectionately',
    'anxiously',
    'ardently',
    'avidly',
    'beautifully',
    'breathlessly',
    'burningly',
    'covetously',
    'curiously',
    'devotedly',
    'eagerly',
    'fervently',
    'fondly',
    'impatiently',
    'keenly',
    'lovingly',
    'passionately',
    'seductively',
    'tenderly',
    'winningly',
    'wistfully',
]
verbs = [
    'adores',
    'attracts',
    'cares for',
    'cherishes',
    'clings to',
    'desires',
    'holds dear',
    'hopes for',
    'hungers for',
    'is wedded to',
    'likes',
    'longs for',
    'loves',
    'lusts after',
    'pants for',
    'pines for',
    'prizes',
    'sighs for',
    'tempts',
    'thirsts for',
    'treasures',
    'wants',
    'wishes',
    'woos',
    'yearns for',
]

def maybe(words):
    if choice([False, True]):
        return ' ' + choice(words)
    return ''

def longer():
    return (
        ' My'
        + maybe(adjectives)
        + ' '
        + choice(nouns)
        + maybe(adverbs)
        + ' '
        + choice(verbs)
        + ' your'
        + maybe(adjectives)
        + ' '
        + choice(nouns)
        + '.'
    )

def shorter():
    return ' ' + choice(adjectives) + ' ' + choice(nouns) + '.'

def body():
    text = ''
    you_are = False
    for i in range(0, 5):
        type = choice(['longer', 'shorter'])
        if type == 'longer':
            text = text + longer()
            you_are = False
        else:
            if you_are:
                text = text[:-1] + ': my' + shorter()
                you_are = False
            else:
                text = text + ' You are my' + shorter()
                you_are = True
    return text

def letter():
    text = choice(first) + ' ' + choice(second) + '\n\n' + \
           textwrap.fill(body(), 80) + '\n\n' + \
           '                            Yours ' + choice(adverbs) + '\n\n' + \
           '                                  M.U.C.' + '\n'
    return text.upper() if (len(argv) > 1 and argv[1] == '-c') else text

print('')
while True:
    print(letter())
    sleep(12.0)
