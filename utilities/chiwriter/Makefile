CHI     = a.chi annotat.chi cl1.chi cl2.chi cl.chi d1.chi d2.chi d3.chi \
          d4.chi d5.chi dd.chi dlit.chi proekt2.chi slast.chi z.chi
TEXT    = a.txt annotat.txt cl1.txt cl2.txt cl.txt d1.txt d2.txt d3.txt \
          d4.txt d5.txt dd.txt dlit.txt proekt2.txt slast.txt z.txt
CVT     = ./chiwriter3.py

all:    $(TEXT)

clean:
	rm -f $(TEXT)

a.txt:  a.chi $(CVT)
	$(CVT) a.chi > $@

annotat.txt: annotat.chi $(CVT)
	$(CVT) annotat.chi > $@

cl1.txt: cl1.chi $(CVT)
	$(CVT) cl1.chi > $@

cl2.txt: cl2.chi $(CVT)
	$(CVT) cl2.chi > $@

cl.txt: cl.chi $(CVT)
	$(CVT) cl.chi > $@

d1.txt: d1.chi $(CVT)
	$(CVT) d1.chi > $@

d2.txt: d2.chi $(CVT)
	$(CVT) d2.chi > $@

d3.txt: d3.chi $(CVT)
	$(CVT) d3.chi > $@

d4.txt: d4.chi $(CVT)
	$(CVT) d4.chi > $@

d5.txt: d5.chi $(CVT)
	$(CVT) d5.chi > $@

dd.txt: dd.chi $(CVT)
	$(CVT) dd.chi > $@

dlit.txt: dlit.chi $(CVT)
	$(CVT) dlit.chi > $@

proekt2.txt: proekt2.chi $(CVT)
	$(CVT) proekt2.chi > $@

slast.txt: slast.chi $(CVT)
	$(CVT) slast.chi > $@

z.txt:  z.chi $(CVT)
	$(CVT) z.chi > $@
