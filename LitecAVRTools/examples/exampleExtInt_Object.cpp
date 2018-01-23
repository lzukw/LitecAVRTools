/*
    exampleExtInts_Object.cpp - Test-Module for ExternalInterrupts.h/.cpp
    This is part of the LitecAVRTools-Library.
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
The ATmega2560 has its INT2 Pin located at Pin PD2. This will be used as 
external Interrupt. Pin PD3 will be used as ordinary input. Both pins
are connected to a button (buttons connected between their pin and GND).

All port B - pins are connected to LEDs (low-level turns on the LEDs).
These LEDs display the value of a global variable `counter` (as a
bit-pattern). The value of `counter` is increased every time an INT2-event
happens (every times the button on PD2 is pushed).

But the interrupt is only enabled, while the button at pin PD3 is not
pressed.
Since buttons are bouncing, pushing the button at pin PD2 may
increase `counter` more than once.
*/

#include <avr/interrupt.h>

#include <stdint.h>
#include <util/delay.h>

#include "GpioPinMacros.h"
#include "ExternalInterrupts.h"


//The global variable. If a variable is changed within an 
//Interrupt-Service-Routine, it must be declared as volatile
volatile uint8_t counter = 2; 


//This is the Interrupt-Service-Routine for INT2-Events. The correct
//Interrupt-vector must be passed to the ISR-macro
ISR( INT2_vect )
{
    counter++;
}


int main()
{
    //Pins PD2 and PD3 are connected to buttons.
    //Set mode of Pins PD3 and PD2 to input with activated internal
    //pullup-resistors
	//A Pin used as external Interrupt, must still be programmed to be an
	//input first!!!
    GpioPinObject buttonPd2 = makeGpioPinObject( GpioPin( D, 2 ) );
    buttonPd2.setModeInputPullup();
    GpioPinObject buttonPd3 = makeGpioPinObject( GpioPin( D, 3 ) );
    buttonPd3.setModeInputPullup();


    //All 8 pins of Port B are connected to LEDs (low-level turns an LED on)

    //set mode of all pins to output
    //put out High-level on every pin
    GpioPortObject ledPort = makeGpioPortObject( GpioPort( B ) );
    ledPort.setMode(0xFF);
    ledPort.writeDigital(0xFF);
    
    //Falling edges occur on pushing the button (rising edges occur on 
    //releasing it). Falling Edges shall cause an Interrupt-Event.
    ExtInt eint2 = ExtInt( 2, EXTINT_FALLING_EDGE );

    //initially enable the External Interrupt 2.
    eint2.enable();
    
    //globally enable interrupts
    sei();
    
    uint8_t lastButtonState = cHigh;
    
    while(1)
    {
        //look for changes of the state of the button on PD3:
        uint8_t actualButtonState = buttonPd3.readDigital();
        if ( actualButtonState != lastButtonState )
        {
            //Button on pd3 has been pressed or released
            if (actualButtonState == cLow)
            {
                //button has been pressed -> disable INT2-Interrupt
                eint2.disable();
            }                
            else
            {
                //button has been released -> enable INT2-Interrupt
                //
                //But first clear a pending interrupt: If button on PD2 has
                //been pushed, while Interrupts were disabled, then the
                //interrupt-event is stored in the CPU in an internal
                //interrupt-Flag. Not clearing the pending interrupt and
                //just enabling interrupts would immediately execute the
                //Interrupt-Service-Routine and therefore increase `counter`.
                //
                //Try the difference by commenting out  the
                //eint2.clearPendingEvent()-method-call
                eint2.clearPendingEvent();
                eint2.enable();
            }                
        }
        lastButtonState = actualButtonState;
        
        //display the value of the global variable counter on the LEDs as a
        //bit-pattern
        ledPort.writeDigital( ~counter );
         
        _delay_ms(100);
    }        
}
