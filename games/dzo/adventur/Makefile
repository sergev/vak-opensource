##########################################################################
#                                                                        #
#   @@     @@@    @   @    @@@@    @  @    @@@@@   @  @    @@@     @@@@  #
#  @  @    @  @   @   @    @       @@ @      @     @  @    @  @    @     #
#  @  @    @  @   @   @    @@@     @ @@      @     @  @    @@@     @@@   #
#  @@@@    @  @    @ @     @       @  @      @     @  @    @  @    @     #
#  @  @    @@@      @      @@@@    @  @      @      @@     @  @    @@@@  #
#                                                                        #
#                             ОБЩИЙ MAKE-FILE                            #
#                                                                        #
##########################################################################

ANAME= ad
INAME= ini

DATA=  adv.common  adv.text  adv.data
CAVE=  cave/adv_action cave/adv_cave   cave/adv_class cave/adv_events \
       cave/adv_messag cave/adv_object cave/adv_vocab


##########
adventure: $(ANAME) $(INAME) $(DATA)

$(ANAME): adv
	cd adv;  make

$(DATA): $(CAVE)
	cd cave;  ../$(INAME)
	cd cave;  mv $(DATA) ..

$(INAME): init
	cd init;  make


########
install:     $(ANAME) $(INAME) $(DATA)
	copy $(ANAME) /usr/games
	strip         /usr/games/$(ANAME)
	mv                     $(DATA)  /usr/lib/games


#####
cave:   $(DATA)
	@echo "если изменена программа 'ini' необходимо сделать 'make ini'"
	mv $(DATA) /usr/lib/games


######
clear:
	-rm -f *.b *~
	-rm -f $(DATA)
	-rm -f $(ANAME)
	-cd cave; rm -f *.b
	-cd adv;  make clear
	-cd init; make clear


#########
allclear:
	-rm -f *.b
	-rm -f $(DATA)
	-rm -f $(ANAME)
	-rm -f $(INAME)
	-cd cave; rm -f *.b
	-cd adv;  make clear
	-cd init; make clear
