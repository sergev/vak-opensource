.PHONY: all html pdf clean

all:    pdf html

pdf:
	pandoc russian.md -o russian.pdf --pdf-engine=tectonic --citeproc --template=template.tex

html:
	pandoc russian.md -o russian.html --standalone --mathjax

clean:
	rm -rf russian.html russian.pdf
