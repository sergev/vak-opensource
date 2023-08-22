Links:
    https://intfiction.org/t/command-line-inform-7-how-to-use-ni-inform6-and-cblorb-by-cli-for-9-3-6m62/50108
    https://www.hoowl.se/getting_started_writing_interactive_fiction_with_inform7.html

First step: ni (compiles Inform 7 code to Inform 6 code)

    glulx:
        ni -noprogress -internal /usr/local/share/inform7/Internal -external "$HOME/Inform" -project "$HOME/Inform/my-game.inform"

    zcode:
        ni -noprogress -format=z8 -internal /usr/local/share/inform7/Internal -external "$HOME/Inform" -project "$HOME/Inform/my-game.inform"

In either case, add -release if building for release, e.g.,

    ni -release -noprogress -internal /usr/local/share/inform7/Internal -external "$HOME/Inform" -project "$HOME/Inform/my-game.inform"

Second step: inform6 (compiles Inform 6 code to either glulx or zcode)

    glulx, testing:
        inform6 -E2wSDG "$HOME/Inform/my-game.inform/Build/auto.inf" "$HOME/Inform/my-game.inform/Build/output.ulx"

    zcode, testing:
        inform6 -E2wSDv8 "$HOME/Inform/my-game.inform/Build/auto.inf" "$HOME/Inform/my-game.inform/Build/output.z8"

    glulx, release:
        inform6 -E2w~S~DG "$HOME/Inform/my-game.inform/Build/auto.inf" "$HOME/Inform/my-game.inform/Build/output.ulx"

    zcode, release:
        inform6 -E2w~S~Dv8 "$HOME/Inform/my-game.inform/Build/auto.inf" "$HOME/Inform/my-game.inform/Build/output.z8"
