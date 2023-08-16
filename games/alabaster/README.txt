Here you can find a script which walks through game "Alabaster".

About the game: https://ifdb.org/viewgame?id=b2g8je1xxtqzei4u

Download and play the game:

    $ wget https://ifarchive.org/if-archive/games/glulx/Alabaster.gblorb
    $ frotz Alabaster.gblorb

Play online:

    https://iplayif.com/?story=https://ifarchive.org/if-archive/games/glulx/Alabaster.gblorb

Walkthrough: https://ifarchive.org/if-archive/solutions/Alabaster_walkthroughs.pdf

For Expect script, you need to build glulxe utility from sources, and
link it with cheapglk library:

    git clone https://github.com/MikulasZelinka/glulxe.git
    git clone https://github.com/erkyrath/cheapglk.git
    make -C cheapglk
    make -C glulxe
