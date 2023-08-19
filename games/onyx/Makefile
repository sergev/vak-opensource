#
# Compile to zcode
#
CONTENTS        = /Applications/Inform.app/Contents
NI              = $(CONTENTS)/MacOS/ni
INFORM6         = $(CONTENTS)/MacOS/inform6
NIFLAGS         = -internal $(CONTENTS)/Resources/Internal -format=z8 #-release

# Testing
INF6FLAGS       = -E2wSDv8

# Release
#INF6FLAGS       = -E2w~S~Dv8

all:            story.z8

clean:
		rm -f story.i6 story.z8

story.i6:       story.ni
		$(NI) $(NIFLAGS) -source $<

story.z8:       story.i6
		$(INFORM6) $(INF6FLAGS) $<
