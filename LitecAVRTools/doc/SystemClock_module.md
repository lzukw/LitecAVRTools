# System Clock module #

This module provides a System-Clock using the 8-Bit-Timer/Counter0. 

## Initialization of the module ##

To use it,
add the files SystemClock.h and SystemClock.cpp to your project, and include
the following headers:

```C
`#include SystemClock.h` 
`#include <avr/interrupt.h>`
```

Initialize the Timer/Counter0 to work as the System-Clock by calling 
`initTimer0AsSystemClock` once after power-up, and enable interrupts globally:

```C
initTimer0AsSystemClock();
sei();
``` 

The system-clock executes an interrupt every 256*64 clock cycles, which is 
approximately every millisecond, when using a 16 MHz-CPU-clock. If other 
Interrupt-Service-Routines consume more promgram-execution-time than
 1 millisecond, system-clock-cycles are lost. Keep the execution time of other Interrupt-
Service-Routines as short as possible!


## Features provided by the module ##

After initialization, four functions can be used:

To delay the program, for example, 100 microseconds, use:
```C
delayMicroseconds(100);
```
This is the only function that can be used without having called 
`initTimer0AsSystemClock` before. If an interrupt is served during the delay, 
the delay-time prolongs, and therefore is not correct anymore. To really 
delay the specified time, first disable interrupts globally, and enable them 
after calling `delayMicroseconds` again (using `sei();` and `cli();` from 
`<avr/interrupt.h>`).

To delay the program, for example, 100 milliseconds, use
```C
delayMilliseconds(100);
```
The System clock must have been initialized before (see previus section), 
otherwise this function hangs in an endless loop. Interrupts don't change 
the overall-delay-time (if Interrupt-Service-Routines don't consume too much execution time, see previous section).

To get the amount of ellapsed time in milliseconds since initializing the 
system-Clock (which happens normally shortly after powering-up the 
microcontroller), use:
```C
unsigned long ellapsedTimeInMilliseconds = millis();
```
The `millis` function returns an unsigned-long-value which has a maximum
value of 2^32-1. After 2^32-1 milliseconds (approximately after 49 days) the 
millis-count overflows, and starts again with 0.

To get the amount of ellapsed time in microseconds since initializing the
system-clock, use:
```C
unsigned long ellapsedTimeInMicroseconds = micros();
```
An overflow of the micros-count occurs after 2^32-1 microseconds 
(approximately 70 hours).



