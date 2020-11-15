module Tests

open Xunit

//
// Calculate Fibonacci numbers.
//
let fibonacci n =
    let rec fibrec n f0 f1 =
        match n with
        | 0 -> f0
        | 1 -> f1
        | _ -> fibrec (n - 1) f1 (f0 + f1)

    fibrec n 0I 1I

[<Fact>]
let fibonacci_0() =
    let result = fibonacci 0
    Assert.Equal(0I, result)

[<Fact>]
let fibonacci_1() =
    let result = fibonacci 1
    Assert.Equal(1I, result)

[<Fact>]
let fibonacci_2() =
    let result = fibonacci 2
    Assert.Equal(1I, result)

[<Fact>]
let fibonacci_10() =
    let result = fibonacci 10
    Assert.Equal(55I, result)

[<Fact>]
let fibonacci_20() =
    let result = fibonacci 20
    Assert.Equal(6765I, result)

[<Fact>]
let fibonacci_100() =
    let result = fibonacci 100
    Assert.Equal(354224848179261915075I, result)

[<Fact>]
let fibonacci_1000() =
    let result = fibonacci 1000
    Assert.Equal(43466557686937456435688527675040625802564660517371780402481729089536555417949051890403879840079255169295922593080322634775209689623239873322471161642996440906533187938298969649928516003704476137795166849228875I, result)

[<Fact>]
let fibonacci_2000() =
    let result = fibonacci 2000
    Assert.Equal(4224696333392304878706725602341482782579852840250681098010280137314308584370130707224123599639141511088446087538909603607640194711643596029271983312598737326253555802606991585915229492453904998722256795316982874482472992263901833716778060607011615497886719879858311468870876264597369086722884023654422295243347964480139515349562972087652656069529806499841977448720155612802665404554171717881930324025204312082516817125I, result)
