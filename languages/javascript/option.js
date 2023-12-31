#!/usr/bin/env node
// Option in JavaScript, funny lambda style
// From: https://juan-gandhi.dreamwidth.org/5595734.html

// Definitions for Boolean Logic
const True = x => y => x
const False = x => y => y
const Bool = b => b ? True : False

// Definitions for pair and projection
const p1 = x => y => x // did you notice? It's True
const p2 = x => y => y // did you notice? It's False
const Pair = x => y => f => f(x)(y)

// Option constructors and operations
const None = Pair(True)("None")
const Some = x => Pair(False)(x)
const isEmpty = x => x(p1)
const FlatMap = x => f => isEmpty(x) (None) ( f(x(p2)))
const Map = x => f => isEmpty(x) (None) (Some(f(x(p2))))
const Filter = x => p => isEmpty(x) (None) (p(x(p2)) (x) (None))

// visualizer
const show = text => xOpt => {
    Map(xOpt)(x => console.log(text + x))
}

// samples
show("Expecting Some ")(Some(42))
show("Expecting None ")(None)

const isPositive = x => Bool(x > 0)
show("0 positive? ")(Filter(Some(0))(isPositive))
show("2 positive? ")(Filter(Some(2))(isPositive))

const dec = n => n > 10 ? Some(n-10) : None

show("expecting None from None: ")(FlatMap( None)(dec))
show("expecting None from 5: ") (FlatMap(Some(5) )(dec))
show("expecting 32 from 42: ") (FlatMap(Some(42))(dec))
