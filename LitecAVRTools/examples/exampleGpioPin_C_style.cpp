/*
    exampleGpioPin_C_style.cpp - example manipulating single GPIO-pins using
    macros (that behave like C-functions).

    For AVR ATMega328p (Arduino Uno) and ATMega2560 (Arduino Mega).
    This is part of the LitecAVRTools library (a fork of the
    AVRTools-library from Igor Mikolic-Torreira).

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
Connect a button between Pin PD2 and GND, and two LEDs (with 220Ohm-Series-
resistors) between VCC and Pins PB0, PB1.
Each Time the button is pressed, the two LEDs toggle. More exactly the LEDs
toggle on releasing the button.
*/

#include <stdint.h>
#include <util/delay.h>

#include "GpioPinMacros.h"

#define buttonPinPD2      GpioPin( D, 2 )
#define ledPinPB0         GpioPin( B, 0 )
//for the the other LED on PB1, every time the macro GpioPin (B,1) is used
//in place (for demonstration purposes)

int main(void)
{
    //configure Button: Pin PD2 is an input with activated pullup-resistor
    //The button is connected between PD2 and GND
	setGpioPinModeInputPullup( buttonPinPD2 );

    //configure Pins PB0 and PB1 (connected to LED0) as output. Turn on the
	//LED on PB0 by writing a Low-Voltage-Level to PB0. Turn off the LED on PB1
	//by writing a High-Voltage-Level to PB0. (The LEDs are connected in series
	//with 220 Ohm-resistors between VCC and the pins).
    setGpioPinModeOutput( ledPinPB0 );
    setGpioPinModeOutput( GpioPin( B, 1 ) );
    writeGpioPinDigital( ledPinPB0, cLow);
    writeGpioPinDigital( GpioPin( B, 1), cHigh);


    uint8_t oldbuttonState = cHigh;

    while (1)
    {
        //check for rising edges on input-pin pd2 (these occur on releasing
        //the button):
        uint8_t buttonState = readGpioPinDigital( buttonPinPD2 );
        if ( oldbuttonState == cLow && buttonState == cHigh)
        {

        	toggleGpioPin( ledPinPB0 );
        	toggleGpioPin( GpioPin( B, 1 ) );

        }

        oldbuttonState = buttonState;
        _delay_ms(100);
    }
}
