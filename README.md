# Thermostat
Source code for the thermostat from "Arduino: A Technical Reference".

Created for chapter 12 of "Arduino: A Technical Reference" by J. M. Hughes

Copyight 2015-2016 John M. Hughes

This is the least polished of the applications. It is also the most
complex. It still needs some rough edges smoothed down.

## Update 18 July 2016

I am continuing to work on the code and get it into its final form. The
last thing to work out is the profile handling. The control state logic
could also use some attention.

## Update 24 July 2016

I decided to put the buttons on the LCD shield to work. The "Select" button
now commands a reset of the stored EEPROM data, effectively putting the
device into an initial state with no saved data. I implemented this when
I discovered that it was possible to get the contents of the EEPROM into
a mess that resulted in NaN (not-a-number) values whilst messing around
with the code, and I had no way to clean out the garbage in the EEPROM.
Now I do.

The remaining buttons are now used to adjust the values obtained from the
DHT22 sensor. The DHT22 datasheet claims a temperature accuracy of +/- 0.5C
and an RH accuracy of between +/- 2%RH and +/- 5%RH. I compared the readings
from the DHT22 to a couple of the digital thermometers in my shop (both a
tethered sensor model and a hand-held thing with a laser dot). To my surprise
the DHT22 agreed with the tethered sensor unit to within a degree for readings
in C. For F readings the variance was somewhat larger but still within
between 1.5 and 2 degrees. The hand-held meter, however, was way off, even
when I put the IR sensor right up on the DHT22. Now I know. At some point I
will probably break out my lab thermometer and see what that reads.

So in order to compensate for variations I decided to use the extra buttons
on the LCD shield. This means I had to drill six small holes through the
front of the thermostat enclosure and activate the pushbutton switches with
a straightened paperclip. But that's OK, I don't plan on fiddling around
with them very often (maybe once a year, if that).

The temperature is adjusted in 0.5 C steps, plus or minus. The humidity is
adjusted in steps of 1% either plus or minus. So if you happen to have, or
have access to, an accurate digital thermometer and a hygrometer, then you
can adjust the thermostat to improve its accuracy. I wouldn't count on the
inexpensive combination units sold on-line, as I suspect that some of them
use a DHT22 or an equivalent. A good meter like those used by HVAC engineers
will set you back anywhere from $150 to $500. If you really need a higher
degree of accuracy then it would be better to borrow one if you can.

Then again, the RH isn't used for anything in the thermostat code (yet), so
a good digital thermometer is probably more useful. I do recommend having
one of those around, and a relatively accurate unit can be had for about $25.
Of course, you could just not worry about accuracy and set the thermostat to
whatever is comfortable for you. That's what it's really all about, anyway.

One last note: The code to handle the LCD shield buttons is in the main file,
Thermostat.ino, in the funciton LCDbutton. I elected to use discrete range
tests rather than the FOR loop approach taken by others. The buttons are
assigned as follows:

 SEL    System Reset (not an MCU reset)
 UP     + 0.5C temp adjust
 DOWN   - 0.5C temp adjust
 LEFT   -1% humidity adjust
 RIGHT  +1% humidity adjust

When any of the four adjustment buttons are pressed the LCD display time-out
is reset and the external DHT22 values display (when it's enabled) is replaced
by two values that show the adjustment parameters. The display returns to
normal operation after a subsequent display time-out. Pressing the rotary
encoder once will bring it back, as usual, but without the adjust values.
The adjustments only apply to the internal DHT22 sensor.

The reset button does what the label says, and resets the MCU.

---
O'Reilly appreciates, but does not require, attribution. An attribution usually
includes the title, author, publisher, and ISBN. For example:

Arduino: A Technical Reference, by John M. Hughes. Copyright 2016 O’Reilly
Media, Inc., ISBN: 978-1-4919-2176-0.

If you feel your use of code examples falls outside fair use or the permission
given above, feel free to contact permissions@oreilly.com.

---

## How to build the code

All you need to do is download the source as a zip file (there is a button
that will do that for you), and then unzip it into a directory called
"Thermostat" in your Arduino "sketchbook" directory.

### Required libraries

For this project you will need the following additional libraries:

- DS1302RTC library by Timur Maksimov
- ClickEncoder library from karl@pitrich.com
- The DHT22 library by Ben Adams

Refer to Chapter 12 for instructions on how to test the code to verify that
it is working correctly with the hardware.

---
> The software and PCB designs in this set of repositories are provided as companion material
> for the book "Arduino: A Technical Reference". The software and design artifacts are provided
> as-is for illustrative purposes and are provided without warranty of any kind. There is no
> guarantee of fitness for any particular application, and no guarantee of correctness. By
> downloading and using these materials the user accepts all risks associated with the
> construction and use of the circuits and devices described in the book and in these design
> files.

---
