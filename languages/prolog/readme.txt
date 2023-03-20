Install prolog as:

    sudo apt install swi-prolog-nox

Try greek dogs as:

    $ swipl greek.pl

    ?- whois(zeus).
    zeus is male
    Their parents are: [cronos,rhea]
    Their kids are: [aeacus,aethlius,aglaea,amphion,apollo,arcas,ares,artemis,athena,calliope,cleophema,clio,dike,dionysus,eirene,enyo,epaphus,erato,ersa,eunomia,euphrosyne,euterpe,hebe,hellen_of_troy,hephaestus,heracles,hermes,iasion,lacedaemon,magnes,melpomene,minos,myrmidon,pandia,pasithea,persephone,perseus,pirithous,pollux,polyhymnia,rhadamanthus,sarpedon,sarpedon_troy,tantalus,terpsichore,thalia,thalia_charity,urania,zethus]
    Their partners are: [aegina,alcemene,antiope,callisto,danae,demeter,dia,electra,erato,europa,eurymedousa,eurynome,hera,io,laodamia,leda,leto,maia,metis,mnemosyne,plouto,protogencia,selene,semele,taygete,themis,thyia]
    Their half siblings are: [chiron]
    Their full siblings are: [demeter,hades,hera,hestia,poseidon]
    Their shortest path to chaos is: [chaos,gaia,rhea,zeus]
    Their ancestors are: [chaos,cronos,gaia,ouranos,rhea]
    Their descendents are: [abas,acamas,achilles,acrisius,actaeon,adonis,aeacus,aegeus,aegisthus,aegyptus,aerope,aeson,aetheopian_guy,aethlius,aethra,aetolus,agamemnon,agave,agenor,agenor_cal,aglaea,alcaeus,alcathous,alcemene,alcidice,alcimenes,aletes,aleus,althea,amphion,amphissus,amphitryon,amyclas,anaxibia,anaxo,andromeda,anticlea,antigone,antiope_amazon,apheidas,apollo,arabius,arcas,arcesius,ares,argus_jason,ariadne,aristaeus,artemis,asclepius,astydamea,astynous,athena,atlanta,atrax,atreus,autolycus,automedusa,autonoe,bellerophon,belus,beroe,bura,cabeiri,cadmus,calais,calliope,calydon,cassiepeia,castor,catreus,cecrops_younger,centaurus,cephalus_of_athens,cepheus_elder,cepheus_younger,chariclo,chione,chrysippus,chrysothemis,cilix,cinyras,cleopatra,cleophema,clio,clytemnestra,coronis,creon,creusa_athens,crisus,cynortas,danae,danaus,deimos,demophon,dike,diomede,dionysus,dorus,dryope,dryops,eirene,elatus,electra_atreus,electryon,endeis,endymion,enyo,epaphus,ephialtes,epicaste,erato,erechtheus,ericthonius,erigone,eriopis,eros,ersa,erysichthon,eteocles,eunomia,euphrosyne,europa,eurydice,eurynome_athenian,eurynomus,eurythoe,eurytion,euterpe,evippus,glaucus_of_lydia,golgos,gorgophone,harmonia,hebe,hedone,hellen_of_troy,hephaestus,heracles,hermes,hermione,hippoboetia,hippodamia,hippolochus,hippolyta,hippolytus,hiscilla,hyacinth,hypermnestra,iambe,iasion,iasus,icarius,ino,iolaus,ion,iphicles,iphiclus,iphigenia,iphimedeia,ismene,ixion,iynx,jason,jocasta,labdacus,lacedaemon,laertes,laius,laodamia,laodice,laophonte,lapithes,learchus,leda,libya,lycurgus,lynceus,magnes,medus,melanippe,meleager,melicertes,melpomene,menelaus,menoeleus,mermeros,minos,morpheus,myrmidon,myrrha,myrtilus,neaera,nessus,niobe,niobids_female,niobids_male,nisos,odysseus,oebalus,oedipus,oeneos,oenomaus,oenopion,orestes,orithyia,orsinome,otus,pan,pandia,pandion_elder,pandion_phineus,pandion_younger,parthenopeus,pasithea,peleus,pelias,pelopia,pelops,penelope,penthesilea,pentheus,penthilus,pereus,persephone,perses_perseide,perseus,phaedra,phaethon,pheres,philomelus,phineus,phlegyas,phobos,phocus_aeacus,phoenix,pirithous,pittheus,pleuron,plexippus,plutus,pollux,polydora,polydorus,polyhymnia,polyneices,polypoetes,porthaon,procris,pylades,rhadamanthus,sandocus,sarpedon,sarpedon_troy,sciron,semele,staphylus,strophius,tantalus,tantalus_jr,telegonus,telemachus,terpsichore,thalia,thalia_charity,theseus,thessalus,thestius,thronia,thyestes,tisamenus,tisander,troezen,tyndareus,tyro,urania,xanthippe,zetes,zethus]
    true .

Greek.pl was downloades from: https://github.com/EStaiman/Greek

Try Fibonacci numbers:

    $ swipl fib.pl

    ?- fib(11, F).
    F = 89 .

    ?- fib(26, F).
    F = 121393 .

Tail-recursive version of Fibonacci numbers:

    $ swipl fib_tail.pl

    ?- fib(11, 0, 1, F).
    F = 89 .

    ?- fib(266, 0, 1, F).
    F = 17427187520417066673081023209641459549125606105821258513 .
