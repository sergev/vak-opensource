#!/usr/bin/env python3

import sys, pexpect, re, itertools, copy

#
# Play game with given control.
# Return the resulting impact velocity (in mph).
#
def play_game(control):
    game = pexpect.spawn('./rocket', encoding='utf-8')
    game.logfile_read = sys.stdout
    game.expect('LBS   FUEL RATE')
    game.setecho(False)

    while True:
        # Read input:
        #
        # TIME,SECS   ALTITUDE,MILES+FEET   VELOCITY,MPH   FUEL,LBS
        #       0             120      0        3600.00     16000.0
        #
        index = game.expect(['K=:', 'AGAIN'])
        if index != 0:
            print()
            match = re.search(r'IMPACT VELOCITY OF   ([0-9.]+) M', game.before)
            #print("match =", match)
            #print("groups =", match.groups())
            #print("group 1 =", match.group(1))
            impact_velocity = match.group(1)
            print("impact_velocity =", impact_velocity)
            return float(impact_velocity)

        word = game.before.split()
        #print(word)

        time_sec = int(word[0])

        # Send fuel rate, LBS.
        if time_sec < 70:
            fuel_rate = 0
        elif time_sec < 150:
            index = (time_sec - 70) // 10
            fuel_rate = control[index]
        else:
            fuel_rate = 0

        print(fuel_rate)
        game.sendline(f"{fuel_rate}")

#
# Given a control and it's score, vary it's item with given index
# to find a better score. Return the new control and score
# (or old values, in case no enhancement has been made).
#
def optimize_control(control, index, score):

    initial_score = score

    # Increase control at given index.
    while control[index] < 200:
        new_control = copy.deepcopy(control)
        new_control[index] += 1
        new_score = play_game(control)
        if new_score > score:
            # Got worse.
            break

        control = new_control
        score = new_score

    if score < initial_score:
        # Found better control.
        return (control, score)

    # Decrease control at given index.
    while control[index] > 8:
        new_control = copy.deepcopy(control)
        new_control[index] -= 1
        new_score = play_game(control)
        if new_score > score:
            # Got worse.
            break

        control = new_control
        score = new_score

    return (control, score)

#
# Play a series of games starting with initial control,
# and optimize it according to the given variant.
# Print the best score (minimal) and appropriate control.
#
def optimize_variant(variant, initial_control, initial_score):

    control = copy.deepcopy(initial_control)
    score = initial_score

    while True:
        last_score = score

        for index in variant:
            (control, score) = optimize_control(control, index, score)

        # Stop when no enhancement anymore.
        if score == last_score:
            break

    # Print the best score.
    if score < initial_score:
        print(score, control, variant)

#
# Try all variants one by one.
#
def do_all_variants():
    variants = list(itertools.permutations([7, 6, 5, 4, 3, 2, 1, 0]))
    #print(variants)

    initial_control = [200, 200, 200, 200, 200, 200, 200, 199]
    initial_score = 340.25 # play_game(initial_control)

    for variant in variants:
        optimize_variant(variant, initial_control, initial_score)

do_all_variants()
