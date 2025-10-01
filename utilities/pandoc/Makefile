.PHONY: all html pdf clean

all:    html pdf

html:
	pandoc example.md -o example.html --standalone --mathjax --citeproc --css=style.css

pdf:
	pandoc example.md -o example.pdf --pdf-engine=tectonic --citeproc --template=custom_template.tex

clean:
	rm -rf _build example.html example.pdf
