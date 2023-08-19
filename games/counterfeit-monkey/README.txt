Here you can find a script which walks through game "Counterfeit Monkey".

About the game: https://ifdb.org/viewgame?id=aearuuxv83plclpl

Download and play the game:

    wget https://github.com/i7/counterfeit-monkey/releases/download/r11.1/CounterfeitMonkey-11.gblorb
    glulxe CounterfeitMonkey-11.gblorb

Play online:

    https://iplayif.com/?story=https://github.com/i7/counterfeit-monkey/releases/download/r11.1/CounterfeitMonkey-11.gblorb

Hints:
    https://intfiction.org/t/counterfeit-monkey-invisiclues/44257
    https://emshort.files.wordpress.com/2013/01/removals-puzzles.jpg

For Expect script, you need to build glulxe utility from sources, and
link it with cheapglk library:

    git clone https://github.com/MikulasZelinka/glulxe.git
    git clone https://github.com/erkyrath/cheapglk.git
    make -C cheapglk
    make -C glulxe
