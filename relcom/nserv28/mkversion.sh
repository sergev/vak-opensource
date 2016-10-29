:
if [ ! -f version.h ]; then
	cat > version.h << EOF
# define VERSION        "2.0"
# define GENERATION     0
EOF
fi

gener=`grep GENERATION version.h | sed 's/[^0-9]*\([0-9]*\).*/\1/'`
newgener=`expr $gener \+ 1`
sed "/GENERATION/s/$gener/$newgener/" version.h > version.h~ &&\
	mv version.h~ version.h
