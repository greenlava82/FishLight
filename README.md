# FishLight
PIC16F18114 code - replacing TopFin RGB LED 5 Gallon Fish Tank Light

For:
[https://www.petsmart.com/fish/tanks-aquariums-and-nets/aquariums/top-fin-custom-colorflowandtrade-aquarium-with-7-color-changing-leds-59426.html?cgid=300129&fmethod=Browse]

I bought this 5 gallon tank for my betta during a year end half-off sale.  I liked the rear filter compartment and the easy water change spout, and I thought the RGB lights would be nice - I intended to use white for daytime, and blue/green for nighttime.  Turns out, the single button user interface for cycling through and selecting colors was...  dumb.  Every time I wanted to choose a color, I'd need to cycle through R/W/B/W/G/W until I found a starting color, then let it fade into a color I liked.  Hold the button for 3 seconds to turn the light on/off, but the initial button press caused the lights to change color.  I thought there must be a better way.  So I opened the little button/brains box and found a simple SOIC-8 microcontroller controlling 3 FETs for RGB.  Throwing the scope on the line, I say a 100Hz PWM controlling each FET.  With this knowledge, I decided to place an inexpensive PIC16F part to do this job in a much better way.  This project requires a bit of soldering, but nothing too fancy.

Steps:
 - Download and compile code in MPLABX v6.15 (or later)
 - Program the PIC16F18114 SOIC-8 (using a Pickit3/4/5 and a chip socket https://t.ly/YKZkt)
 - Remove U1, C2, R4 (save the resistor and capacitor for the next step
 - Cut the line between C2 and R4, then swap the parts (the PIC16F I chose switches the MCLR and VCC pins from the stock MCU) so that R4 is p by pin4 and C2 is by pin 1
 - Place the PCB back in the box and test lighting.

Usage:
The programming on this new chip is designed with several states:
 -> Power On
 -> Sunrise (30 minute RGB fade-in)
 -> Daylight (12 Hour Full Brightness (actually, 85% in my code)
 -> Sunset (30 Minute RGB Fade out to...)
 -> Twilight (2 hr Blue/Green at 20%
 -> Nighttime - Light Off until Sunrise

Any of these timers are configurable at compile time by changing the TIME_x defines at the top of main.c.  the system timer runs on a 100ms loop, so time values are *10

Because the PIC16F's internal 32khz oscillator is prone to clock drift, I use an external Wifi outlet with a schedule to turn might light on each morning, and off at night.  This allows the Sunrise to occur at the same time every morning
