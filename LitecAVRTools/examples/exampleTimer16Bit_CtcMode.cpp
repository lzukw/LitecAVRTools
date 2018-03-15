/*
    exampleTimer16Bit_CtcMode.cpp - Test-Module for Timer_16Bit.h and
    Timer_16Bit.cpp for Timer-mode 4 (CTC mode with OCR1A as TOP-value)

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
    A software-PWM-signal is created with Timer/Counter 1 in CTC-mode.
    The software PWM-signal is put out on Port C, pin 1, and has a period of
    20ms (if main-oscillator-frequency is 16MHz). The duty-cycle starts at
    5% and increases within 11 seconds to 10%. Then it starts again
    from 5% .

    Connect a LED or an RC-servo to pin PC1 to test this example.
 */

#include <avr/interrupt.h>
#include <stdint.h>

#include "GpioPinMacros.h"
#include "SystemClock.h"
#include "Timer16Bit.h"
#include "Usart.h"


GpioPinObject pwmPin  = makeGpioPinObject( GpioPin( C, 1 ) );
TimerCounter16Bit tc1 = makeTimerCounter16BitObject( 1 );
Usart usart0 = makeUsartObject( 0 );

volatile uint8_t compAIntCount=0;
volatile uint8_t compBIntCount=0;


int main()
{
    uint16_t offValue=2000;

    pwmPin.setModeOutput();
    pwmPin.writeDigital( c_High );

    usart0.init( 9600 );
    usart0.usartPrintf("starting program...\r\n");

    initTimer0AsSystemClock();
    sei();

    //Timer1 in CTC-mode, prescaler-value 8, OCR1A-value (TOP-value) set to 39999,
    //Compare-match-A- and Compare-match-B-Interrupts enabled, OCR1B (compare-match-B-register) set initially to 1000
    tc1.setMode( T16_CTC_OCRNA );
    tc1.setTopValue(39999); //with prescaler 8 and 16MHz this results in 20ms period.
    tc1.setCompareMatchValue( T16_COMP_B, offValue ); //start with 5% duty cycle
    tc1.enableInterrupts( T16_INT_COMP_MATCH_A | T16_INT_COMP_MATCH_B );
    tc1.selectClockSource( T16_PRESC_8); //start Timer with prescaler 8.

    while(1)
    {
        usart0.usartPrintf("CompA-Interrupts: %du, CompB-Interrupts: %du\r\n", compAIntCount, compBIntCount );
        delayMilliseconds(1000);

        //just change the Compare-Match-Value for Channel B (content of OCR1B-regsister) between 2000 and 4000
        offValue += 200;
        if (offValue > 4000)
            offValue = 2000;
        tc1.setCompareMatchValue( T16_COMP_B, offValue );
    }
}


//This Interrupt-Service-Routine is executed when Timer/Counter1 reached its TOP-value (39999, content of OCR1A), and
//started again from 0.
ISR(TIMER1_COMPA_vect)
{
    pwmPin.writeDigital( c_High );
    compAIntCount++;
}


//This Interrupt-Service-Routine is executed, when the actual
//Timer1-count-value is equal to the Compare-match-value of Channel B (OCR1B)
ISR(TIMER1_COMPB_vect)
{
    pwmPin.writeDigital( c_Low );
    compBIntCount++;
}
