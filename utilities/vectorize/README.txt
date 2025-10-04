Here is an example of vectorizing a color PNG image into SVG picture.

1. Download input image:

    wget https://pub.sergev.org/media/kozak.png

2. Run scripts in sequence:

    ./1-reduce.sh
    ./2-split.sh
    ./3-trace.sh
    ./4-merge.py
    ./5-copyright.py

See result in file `kozak.svg`.

https://pub.sergev.org/media/kozak.svg
