#
# Run:
#   julia hello.jl
#
using UnicodePlots
using Distributions
f() = rand(Normal())

println("hello, world")
@show f()
println(lineplot(1:10, (1:10).^2))
