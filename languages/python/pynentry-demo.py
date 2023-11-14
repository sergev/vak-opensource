#!/usr/bin/env python3
#
# Demo: how pynentry works.
#
import pynentry

with pynentry.PynEntry(executable='/opt/homebrew/bin/pinentry-mac') as p:
    #
    # Show message.
    #
    p.description = 'Hello there!'
    p.show_message()

    #
    # Ask for password.
    #
    p.description = 'Enter a password.\n Choose Wisely!'
    p.prompt = 'PASS>'
    try:
        passwd = p.get_pin()
    except pynentry.PinEntryCancelled:
        print('Cancelled?! Goodbye.')
        exit()

    #
    # Confirm password.
    #
    p.ok_text = 'yep!'
    p.cancel_text = 'nope!'
    p.description = f'CAN YOU CONFIRM YOUR SUPER SECRET PASSWORD IS {passwd}?'
    if p.get_confirm():
            print(f'password {passwd} saved!')
    else:
            print(f'Too bad, so sad')
