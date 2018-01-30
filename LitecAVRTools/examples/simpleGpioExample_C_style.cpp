/*
    simpleGpioExample_C_style.cpp - example included in README.md using
    macros (that behave like C-functions).

    This is part of the LitecAVRTools library.

    Copyright (c) 2018 Wolfgang Zukrigl

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
    Connect a push-button between GPIO-Pin PD2 (pin number 2 of port D) and
    GND. Connect (up to) 4 LEDs to the GPIO-Pins PA0, PA1, PA2 and PA3.
    The LEDs must be connected in series with a resistor between VCC and the
    corresponding pin.
 */

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
        writeGpioPinDigital( GpioPin ( A, ledNo), cHigh );
    }

    //turn on LED number <actualLed>
    writeGpioPinDigital( GpioPin( A, actualLed ), cLow );

    while(1)
    {
        //is button pushed (produces a Low-voltage-level)?
        //if yes: switch to next LED
        if (readGpioPinDigital( buttonPin ) == cLow )
        {
            //instead of
            //writeGpioPinDigital( GpioPin( A, actualLed ), cHigh) :
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

