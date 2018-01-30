/*
    exampleGpioPort_C_style.cpp - example manipulating all 8 pins of a
    GPIO-port using macros (that behave like C-functions).

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
Connect a button between Pins PD3, PD2 and GND, and eight LEDs (with
220Ohm-Series-resistors) between all Pins of port B and VCC. The two buttons
control the pattern, that is displayed on the LEDs.
*/

#include <stdint.h>
#include <util/delay.h>

#include "GpioPinMacros.h"


#define ledPort         GpioPort( B )
//For the button-Port no macro is defined, but GpioPort(D) is used in place (for
//demonsatration purposes).

int main(void)
{

    //instantiate a Gpio-Port-object for Port B (connected to LEDs)
    GpioPortObject leds = makeGpioPortObject( ledPort );
    //make all 8 pins of the leds-Port to outputs: The mode argument is
    //0xFF=0b11111111, so all pins are programmed to outputs. For the
    //mask-argument, the default-value (0xFF) is used, so the mode of
    //all 8 Bits is modified.
    leds.setMode(0xFF);
    //set all Pins to high --> turn off LEDs.
    leds.writeDigital(0xF0);


    GpioPortObject buttons = makeGpioPortObject( GpioPort( D ) );
    //In the mask-argument 0x0C = 0b00001100 the bits number 2 and 3 are set,
    //so only Pins PD2 and PD3 are affected. In the mode-Argument these bits
    //are zero, so both GPIO-Pins are programmed as inputs (the mode of the
    //other pins is not modified).
    buttons.setMode( 0x00, 0x0C );
    //activate pullup-Resistors on PD2 and PD3. Bits 2 and 3 in the pullup- and
    //in the mask-Argument are 1, so only Pins PD2 amd PD3 are affected.
    //The 1-Bits in the pullup-Argument turn on the internal pullup-Resistors.
    buttons.setPullup( 0x0C, 0x0C );


    while (1)
    {
        //Are both buttons on pins PD3 and PD2 pressed?
        // mask 0x0C = 0b00001100 --> read in voltage-levels of both pins
        // low-levels means, that buttons are pressed
        if ( buttons.readDigital(0x0C) == 0x00 )
        {
            //toggle all LEDs: mask is 0xFF --> voltage-levels of all 8 Pins
            //of port B are toggled
            leds.toggle();
        }
        //Is the button on Pin PD3 pressed?
        //mask 0x08 = 0b00001000 --> Just read in voltage-level of PD3
        else if ( buttons.readDigital(0x08) == 0x00)
        {
            //put out high-level on Pin PB7 and low-level on PB6
            //(value is 0x80=0b10000000). LED on PB7 is off, LED on PB6
            //is on. Leave the other pins unchanged (mask is 0xC0=0b11000000)
            leds.writeDigital( 0x80, 0xC0 );
        }

        //Is the button on Pin PD2 pressed?
        // mask 0x04 = 0b00000100 --> Just read in voltage-level of PD2
        else if ( buttons.readDigital(0x04) == 0x00)
        {
            //mask is 0xFF = 0b11111111 --> all pins are affected
            //voltageLevels is 0xF8 = 0b11111000
            //Put out High-level on Pins PB7...PB3 and low-levels on PB2...PB0
            //--> LEDs on PB2...PB0 are on, others off.
            leds.writeDigital( 0xF8, 0xFF);
        }
        else //no button is pressed
        {
            //mask is 0xFF = 0b11111111 --> all pins are affected
            ////voltageLevels is 0x55 = 0b01010101
            //--> High-levels on Pins PB0, PB2, PB4, PB6 (these LEDs are off)
            //--> Low-levels on Pins PB1, PB3, PB5, PB7 (these LEDs are on)
            leds.writeDigital( 0x55, 0xFF );
        }

        _delay_ms(100);
    }
}
