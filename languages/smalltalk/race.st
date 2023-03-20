#!/usr/bin/env gst -g
"Simulate a race with random messages        "
"(RTK, 17-Apr-2021, last update: 18-Apr-2021)"

"Animal is a base class"
Object subclass: Animal [
    | increment distance letter |

    init: inc letter: l [
        letter := l.
        increment := inc.
        distance := 0
    ]

    getDistance [ ^distance ]

    print [
        (distance-1) timesRepeat: [  '-' display ].
        letter displayNl
    ]
]

"Define subclasses"
Animal subclass: Bird [
    init [ super init: 6 letter: 'B' ]
    fly [ distance := distance + increment ]
    doesNotUnderstand: msg [ ]
]

Animal subclass: Wolf [
    init [ super init: 5 letter: 'W' ]
    run [ distance := distance + increment ]
    doesNotUnderstand: msg [ ]
]

Animal subclass: Frog [
    init [ super init: 4 letter: 'F' ]
    hop [ distance := distance + increment ]
    doesNotUnderstand: msg [ ]
]

Animal subclass: Snail [
    init [ super init: 3 letter: 'S' ]
    crawl [ distance := distance + increment ]
    doesNotUnderstand: msg [ ]
]

"Create and initialize the objects"
animals := Array new: 4.
animals at: 1 put: (Bird new).
animals at: 2 put: (Wolf new).
animals at: 3 put: (Frog new).
animals at: 4 put: (Snail new).
1 to: 4 do: [ :i | (animals at: i) init ].

"Run the race"
moves := #('fly' 'run' 'hop' 'crawl').

50 timesRepeat: [
    m := moves at: ((1 to: 4) atRandom).
    1 to: 4 do: [ :i | (animals at: i) perform: m asSymbol ].
].

"Show the results"
Transcript cr; show: 'Race results:'; cr; cr
1 to: 4 do: [ :i | (animals at: i) print ].
'' displayNl.

"Declare the winner"
dist := Dictionary new.
1 to: 4 do: [ :i |
    animal := animals at: i.
    dist at: animal put: animal getDistance
].

d := (animals at: 1) getDistance.
winner := (animals at: 1) class displayString.
dist associationsDo: [ :pair |
    ((pair value) > d) ifTrue: [
        d := pair value.
        winner := (pair key) class displayString.
    ]
].

Transcript show: 'The winner is '; show: winner; cr; cr.
