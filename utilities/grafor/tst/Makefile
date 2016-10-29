# 386/ix with Green Hills Fortran
# LIBGRF  = libgrafor.a
# F77     = gf77
# CFLAGS  = -O
# FFLAGS  = -OM
# LDFLAGS =
# LIBS    = ../libgrafor.a
# INSLIB  = /usr/greenhills/BIN/386/lib

# Vax under Ultrix
LIBGRF  = libgrafor.a
F77     = gfortran
CFLAGS  = -O
FFLAGS  = -O
LDFLAGS =
LIBS    = ../libgrafor.a
#INSLIB  = /lib

# Convex under BSD 4.3
# LIBGRF  = libgrafor.a
# F77     = fc
# CFLAGS  = -O
# FFLAGS  = -O2
# LDFLAGS =
# LIBS    = -lgrafor
# INSLIB  = /lib

# Xenix with Microsoft Fortran 286
# LIBGRF  = Llibgrafor.a
# F77     = cl
# CFLAGS  = -O -DMSF -Ml -Me -M2
# FFLAGS  = -Ox -FPi
# LDFLAGS = -Ml -i -M2
# LIBS    = -lgrafor -link -S 256
# INSLIB  = /lib

TSTS    = sample curve card bant pup sphere cyl cyl1 demos gist graf mfti\
	mfti2 mftiwr mfti1 surf symset

all:    $(TSTS)

pcl:	$(TSTS)
	for i in $(TSTS); do rm -f $$i.pcl && ./$$i && metaplot -j grafor.mtf > $$i.pcl; done

mtf:    sample.mtf curve.mtf card.mtf bant.mtf pup.mtf sphere.mtf cyl.mtf\
	cyl1.mtf demos.mtf gist.mtf graf.mtf mfti.mtf mfti2.mtf mfti1.mtf\
	surf.mtf symset.mtf

sample.mtf:     sample
	sample  >sample.mtf

curve.mtf:      curve
	curve   >curve.mtf

card.mtf:       card
	card    >card.mtf

bant.mtf:       bant
	bant    >bant.mtf

pup.mtf:        pup
	pup     >pup.mtf

sphere.mtf:     sphere
	sphere  >sphere.mtf

cyl.mtf:        cyl
	cyl     >cyl.mtf

cyl1.mtf:       cyl1
	cyl1    >cyl1.mtf

demos.mtf:      demos
	demos   >demos.mtf

gist.mtf:       gist
	gist    >gist.mtf

graf.mtf:       graf
	graf    >graf.mtf

mfti.dat:       mftiwr
	mftiwr

mfti.mtf:       mfti mfti.dat
	mfti    >mfti.mtf

mfti2.mtf:      mfti2 mfti.dat
	mfti2   >mfti2.mtf

mfti1.mtf:      mfti1 mfti.dat
	mfti1   >mfti1.mtf

surf.mtf:       surf
	surf    >surf.mtf

symset.mtf:     symset
	symset  >symset.mtf

symset: symset.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

sample: sample.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

curve:  curve.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

cyl:    cyl.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

cyl1:   cyl1.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

demos:  demos.f ../ftn/view.f
	$(F77) $(FFLAGS) demos.f -o $@ $(LIBS)

gist:   gist.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

graf:   graf.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

mfti:   mfti.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

mftiwr: mftiwr.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

surf:   surf.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

mfti1:  mfti1.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

mfti2:  mfti2.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

card:   card.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

bant:   bant.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

sphere: sphere.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

pup:    pup.f
	$(F77) $(FFLAGS) $? -o $@ $(LIBS)

.f.o:
	$(F77) -c $(FFLAGS) $<

.f.out:
	$(F77) $(FFLAGS) $< $(LIBS) -o $@

clean:
	rm -f *~ *.o $(TSTS) *.dat z*

.SUFFIXES: .o .c .f .s .out
