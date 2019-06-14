#!/usr/bin/env julia
#
# Run:
#   julia times.jl
#
using Printf

for i in 2:9
    @printf("%4d %4d %4d %4d %4d %4d %4d %4d\n",
        i*2, i*3, i*4, i*5, i*6, i*7, i*8, i*9)
end
