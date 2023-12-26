#------------------------------------------------------------------------------------------------------------------------
# Importing Libraries
#------------------------------------------------------------------------------------------------------------------------

import time, math
from machine import Pin


#------------------------------------------------------------------------------------------------------------------------
# Defining Functions
#------------------------------------------------------------------------------------------------------------------------

# LDR Signal Callback; LDR == 0 when daytime, LDR == 1 when night time
def ldr_callback(pin):
    global isDaytime
    if LDR_sense.value() == 1:
        isDaytime = False
        road_lights.value(1)
    elif LDR_sense.value() == 0:
        isDaytime = True
        road_lights.value(0)

# Pedestrian Walk Signal Callback
def walk_callback(pin):
    if LDR_sense.value() == 1:
        night_sequence()

# Car Signal Callback
def car_callback(pin):
    # If car signals triggered during night, start night traffic light sequence
    if (car_signal_2.value() == 1 and LDR_sense.value() == 1) or (car_signal_4.value() == 1 and LDR_sense.value() == 1):
        night_sequence()

# Function to run night time traffic sequence
def night_sequence():
    # Set Main Street traffic lights (No.2 and No.4) to yellow (STATE 1)
    G_Main_St.value(0)
    Y_Main_St.value(1)
    time.sleep_ms(Y_timing)

    # Set Main Street light (No.2 and No.4) red, Small Avenue light (No.1 and No.3) green (STATE 2)
    Y_Main_St.value(0)
    R_Main_St.value(1)
    G_Small_Ave.value(1)
    R_Small_Ave.value(0)
    time.sleep_ms(G_timing_night_Small_Ave)

    # Set Small Avenue (No.1 and No.3) traffic lights to yellow (STATE 3)
    G_Small_Ave.value(0)
    Y_Small_Ave.value(1)
    time.sleep_ms(Y_timing)

    # Set Small Avenue light (No.1 and No.3) red, Main Street lights (No.2 and No.4) green (STATE 4)
    Y_Small_Ave.value(0)
    R_Small_Ave.value(1)
    R_Main_St.value(0)
    G_Main_St.value(1)

    # Wait 5 seconds before allowing traffic light change again
    time.sleep_ms(5000)

# Function to run daytime traffic sequence
def day_sequence():
    # wait some time before changing the traffic lights
    time.sleep_ms(G_timing_day_Main_St)

    # Set Main Street traffic lights (No.2 and No.4) to yellow (STATE 1)
    G_Main_St.value(0)
    Y_Main_St.value(1)
    time.sleep_ms(Y_timing)

    # Set Main Street light (No.2 and No.4) red, Small Avenue light (No.1 and No.3) green (STATE 2)
    Y_Main_St.value(0)
    R_Main_St.value(1)
    G_Small_Ave.value(1)
    R_Small_Ave.value(0)
    time.sleep_ms(G_timing_day_Small_Ave)

    # Set Small Avenue (No.1 and No.3) traffic lights to yellow (STATE 3)
    G_Small_Ave.value(0)
    Y_Small_Ave.value(1)
    time.sleep_ms(Y_timing)

    # Set Small Avenue light (No.1 and No.3) red, Main Street lights (No.2 and No.4) green (STATE 4)
    Y_Small_Ave.value(0)
    R_Small_Ave.value(1)
    R_Main_St.value(0)
    G_Main_St.value(1)


#------------------------------------------------------------------------------------------------------------------------
# Initializations
#------------------------------------------------------------------------------------------------------------------------

# Road Lights
road_lights = Pin(0, Pin.OUT)

# Traffic Lights installed on corners "No.1" and "No.3"
G_Small_Ave = Pin(1, Pin.OUT)  # Green light
Y_Small_Ave = Pin(2, Pin.OUT)  # Yellow light
R_Small_Ave = Pin(3, Pin.OUT)  # Red light

# Traffic Lights installed on corners "No.2" and "No.4"
G_Main_St = Pin(4, Pin.OUT)  # Green light
Y_Main_St = Pin(5, Pin.OUT)  # Yellow light
R_Main_St = Pin(6, Pin.OUT)  # Red light

# Light Dependent Resistor (LDR)
LDR_sense = Pin(11, Pin.IN)  # LDR signal pin

# Car and Walk Signals
car_signal_2 = Pin(7, Pin.IN)   # Car signal on corner "No.2"
car_signal_4 = Pin(8, Pin.IN)   # Car signal on corner "No.4"
walk_signal_2 = Pin(9, Pin.IN)  # Pedestrian walk signal on corner "No.2"
walk_signal_4 = Pin(10, Pin.IN) # Pedestrian walk signal on corner "No.4"

# Setting up interrupt requests for LDR Sensor, walk, and car signals
LDR_sense.irq(handler=ldr_callback, trigger=Pin.IRQ_FALLING | Pin.IRQ_RISING)  # Callback on signal change
walk_signal_2.irq(handler=walk_callback, trigger=Pin.IRQ_FALLING)  # Callback on pedestrian walk signal change
walk_signal_4.irq(handler=walk_callback, trigger=Pin.IRQ_FALLING)  # Callback on pedestrian walk signal change
car_signal_2.irq(handler=car_callback, trigger=Pin.IRQ_FALLING)  # Callback on car signal change
car_signal_4.irq(handler=car_callback, trigger=Pin.IRQ_FALLING)  # Callback on car signal change

# Initializing street light states
G_Small_Ave.value(0)
Y_Small_Ave.value(0)
R_Small_Ave.value(1)
G_Main_St.value(1)
Y_Main_St.value(0)
R_Main_St.value(0)
road_lights.value(0)

# Add a flag to check if it's daytime or not
isDaytime = False

# Initial LDR check; checks for LDR state at the start of the program
if LDR_sense.value() == 0:
    isDaytime = True
    road_lights.value(0)

# Adjust the day sequence traffic light timing here (milliseconds)
G_timing_night_Small_Ave = 5000
G_timing_day_Small_Ave = 7000
G_timing_day_Main_St = 15000
Y_timing = 3000


#------------------------------------------------------------------------------------------------------------------------
# Main Loop
#------------------------------------------------------------------------------------------------------------------------

while True:
    if isDaytime:  #if it is daytime run the daytime traffic light sequence
        day_sequence()

    else:  # Night mode (in standby mode until walk or car interrupt occurs)
        time.sleep(0)  # sleep for a while to reduce CPU usage (seconds)
