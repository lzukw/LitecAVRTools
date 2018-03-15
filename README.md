# LitecAVRTools #

## Overview ##

This is a library for AVR-Microcontrollers. It is used to teach the basics 
of microcontrollers (GPIO-pins, Interrupts, Timer/Counter and RS232-Interface) 
to students on a technical high-school.

The LitecAVRTools-library is highly inspired by the 
[AVRTools-library](https://github.com/igormiktor/AVRTools) from 
Igor Mikolic-Torreira.

For now the ATmega2560 and the Atmega328p are supported.

The LitecAVRTools share some design goals with the original AVRTools-Library:

- avrgcc is used as compiler, replacing the Arduino-IDE and its core-library. 
  Another IDE can be used, such as Atmel-Studio. Alltough the Arduino-IDE 
  could still be used, if one doesn't provide a `setup()` and a `loop()` 
  function, but simply a `main()` function. 
  Therfore every IDE is suitable, that allows programming Arduino-scetches, 
  for example Microsoft-Visual-Studio with the Visual-Micro-plugin.
- Programming should be nearly as comfortable as using the Arduino-IDE 
  and its core library. 
- Bits in special-function-registers of the Microcontroller don't need to be
  manipulated directly. Instead functions (macros) with meaningful names are 
  used. For example, to make pin PB3 (pin number 3 of Port B) an output, it 
  is not necessary to write `DDRB |= (0x01<<3);`, but one can write 
  `setGpioPinModeOutput( GpioPin( B, 3) );`

However, the LitecAVRTools-library has some different design-goals compared 
to the AVRTools-Library:

- Pins are not named according to the names used on Arduino-Mega- and 
  Arduino-Uno-Boards. The pin-names used by LitecAVRTools follow the
  datasheet of the ATmega-Microcontrollers. For example pin number 0 of
  port A in the datasheet is referred to as PA0.
- It is possible to program a whole port (8 pins at once). For example
  this is useful for driving a seven-segment-display.
- PWM-outputs and analog-inputs are (will be) supported by the library.
  But this is not done by programming pins. Instead one has to program a 
  Timer/Counter to use PWM-outputs, or the Analog-to-Digital-Converter
  to use analog inputs.
- It is necessary to know/find information contained in the datasheet of 
  the microcontroller. For example if a PWM-output 
  is used, it is necessary to know, that the output-compare-pin A of 
  Timer/Counter1 (OC1A) on the Atmega2560 is an alternative function of 
  Pin PB5, and that this pin must first be programmed as ordinary output.

## Simple Example using C-Syntax ##

Add the file GpioPinMacros.h to your project. This is the only file needed
for the GPIO-module of LitecAVRTools.

To use the following example, connect a push-button between GPIO-pin PD2 (the
pin number 2 of Port D) and GND. Connect (up to) four LEDs between the pins
PA0...PA3 and VCC. Don't forget the series resistors!

Each time the push-button is pressed, the next LED lights up.

```C
#include <util/delay.h>

#include "GpioPinMacros.h"

#define buttonPin     GpioPin( D, 2 )

int main()
{
    int ledNo;
    int actualLed = 0;

    //
    //initialize button and LEDs
    //
    setGpioPinModeInputPullup( buttonPin );

    for (ledNo = 0; ledNo<4; ledNo++)
    {
        setGpioPinModeOutput( GpioPin ( A, ledNo ) );
        //High-level turns LED off:
        writeGpioPinDigital( GpioPin ( A, ledNo), c_High );
    }

    //turn on LED number <actualLed>
    writeGpioPinDigital( GpioPin( A, actualLed ), c_Low );

    while(1)
    {
        //is button pushed (produces a Low-voltage-level)?
        //if yes: switch to next LED
        if (readGpioPinDigital( buttonPin ) == c_Low )
        {
            //instead of
            //writeGpioPinDigital( GpioPin( A, actualLed ), c_High) :
            setGpioPinHigh( GpioPin ( A, actualLed) ); //turn off Led
            actualLed++; //switch to next LED
            if ( actualLed > 3 )
                actualLed = 0;
            setGpioPinLow( GpioPin ( A, actualLed ) ); //turn on Led
        }

        _delay_ms(150);
    }

    return 0;
}
```

Beginners watch out: The functions used in this program are not functions but 
macros.If you have Syntax-Errors, normally avrgcc does not report them in your
program, but in GpioPinMacros.h

## Further documentation ##

Further documentation can be found in the folder named doc. There is a 
.md-File for each module. 

To use the more sophisticated features of the AVR-microcontrollers (for 
example PWM with Timer/Counter), see the relevant section of the datasheet 
of the ATmega328p / Atmega2560. 

