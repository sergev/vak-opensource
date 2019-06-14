using Plots
using StatsPlots
using Distributions
gr()
display(plot(Normal(3,5),lw=3))
savefig("normal.png")
