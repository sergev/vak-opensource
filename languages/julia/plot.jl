using Plots
x = 1:10; y = rand(10); # These are the plotting data
gr()
display(plot(x,y))
savefig("myplot.png")
