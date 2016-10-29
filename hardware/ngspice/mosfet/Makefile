
all:    invertor.cir

# Convert schematics into Spice netlist
invertor.cir: invertor.sch
	gnetlist -g spice-sdb -o $@ $<

# Run simulation
run:    invertor.cir test.cir
	ngspice test.cir

# Edit schematics
edit:
	gschem invertor.sch
