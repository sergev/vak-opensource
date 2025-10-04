.PHONY: all html pdf clean

all:    pdf html

pdf:
	pandoc example.md -o example.pdf --pdf-engine=tectonic --citeproc --template=template.tex

html:
	pandoc example.md -o example.html --standalone --mathjax

clean:
	rm -rf example.html example.pdf
