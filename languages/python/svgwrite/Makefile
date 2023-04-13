all:    picture1.png

clean:
	rm -f *.png *.svg

picture1.png: picture1.py
	./picture1.py
	svg2png picture1.svg $@
