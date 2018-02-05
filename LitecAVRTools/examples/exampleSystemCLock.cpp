/*
    exampleSystemClock.cpp - Example/Test for SystemClock-module consisting of
    SystemCLock.h and SystemClock.cpp

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

#include <avr/interrupt.h>

#include "Usart.h"
#include "SystemClock.h"

int main()
{
    //create usart0-object and initialize it to 9600 baud, 8N1
    Usart usart0 = makeUsartObject( 0 );
    usart0.init( 9600 );

    //initialize the System-clock (Timer0), and globally enable interrupts.
    initTimer0AsSystemClock();
    sei();

    unsigned long startMillis = 0;

    while(1)
    {
        //The while(1)-loop shall be repeated every 1000 ms
        while( millis() - startMillis < 1000 ) { /* empty */ }
        startMillis += 1000;

        unsigned long firstMicros = micros();
        delayMilliseconds( 1 );
        delayMicroseconds( 1000 );
        unsigned long secondMicros = micros();

        usart0.usartPrintf( "time difference  = %lu\r\n", secondMicros - firstMicros );

    }

    return 0;
}
