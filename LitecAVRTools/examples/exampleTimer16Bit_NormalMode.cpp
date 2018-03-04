/*
    exampleTimer16Bit_NormalMode.cpp - Test-Module for Timer_16Bit.h and
    Timer_16Bit.cpp for Timer-mode 0 (normal mode)

    This is part of the LitecAVRTools library.

    Copyright (c) 2018 Wolfgang Zukrigl

    Some parts of this file are copied from AVRTools-Project, which is
    Copyright (c) 2014 Igor Mikolic-Torreira.

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
    A software-PWM-signal is created with Timer/Counter 1 in normal mode
    Connect a LED with a series-resistor between Gpio-Pin PB0 and GND
    (or VCC, if you like).

    For the first 30 seconds the PWM-frequency is fosc/1024/65536, which
    results in a 4.19 seconds PWM-period (with fosc=16MHz) , and the LED
    blinks with 50% duty cycle.

    Then for 30 seconds the PWM-frequency is set to fosc/1/65536, which is
    244Hz (with fosc=16Mhz), and the duty-cycle is decreased during this
    30 seconds from 100% down to 0%

 */

#include <avr/interrupt.h>

#include "GpioPinMacros.h"
#include "SystemClock.h"
#include "Timer16Bit.h"

GpioPinObject ledPin  = makeGpioPinObject( GpioPin( B, 0 ) );
TimerCounter16Bit tc1 = makeTimerCounter16BitObject( 1 );

int main()
{
    ledPin.setModeOutput();
    initTimer0AsSystemClock(); //for delayMilliseconds() in main()
    sei();

    //Timer1 in normal-mode, prescaler-value 1024, OCR1A-value set to
    //0xFFFF/2 = 0x7FFF. Overflow- and Compare-match-A-Interrupt enabled
    tc1.setMode( T16_NORMAL );
    tc1.selectClockSource( T16_PRESC_1024 );
    tc1.setCompareMatchValue( T16_COMP_A, 0x7FFF );
    tc1.clearPendingInterruptEvents( T16_INT_OVERFLOW | T16_INT_COMP_MATCH_A );
    tc1.enableInterrupts( T16_INT_OVERFLOW | T16_INT_COMP_MATCH_A );

    delayMilliseconds(30000);

    tc1.setMode( T16_NORMAL );
    tc1.selectClockSource( T16_PRESC_1 );
    int i;
    for (i=0; i<30; i++)
    {
        tc1.setCompareMatchValue( T16_COMP_A, 64444 - 2184*i);
        delayMilliseconds(1000);
    }

    while(1) { }
}


//This Interrupt-Service-Routine is executed on Timer1-Overflow (in normal
//mode, this is when the actual count-value wraps from 0xFFFF to 0.
ISR(TIMER1_OVF_vect)
{
    //Turn on LED
    ledPin.writeDigital( c_High );
}


//This Interrupt-Service-Routine is executed, when the actual
//Timer1-count-value is equal to the Compare-match-value of Channel A (OCR1A)
ISR(TIMER1_COMPA_vect)
{
    //Turn off LED
    ledPin.writeDigital( c_Low );
}
