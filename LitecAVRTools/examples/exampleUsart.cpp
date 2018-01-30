/*
    exampleUsart.cpp - Example/Test for Usart-module consisting of
    Usart.h and Usart.cpp

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

#include "Usart.h"
#include <util/delay.h>

int main()
{
    //create usart0-object and initialize it to 9600 baud, 8N1 (8 data bits,
    //no parity-bit, 1 stop-bit)
    Usart usart0 = makeUsartObject( 0 );
    usart0.init(9600, cUsart_8N1); //cUsart_8N1 is default value ...could be omitted

    while(1)
    {
        char  c;
        for (c='0'; c<='9'; c++)
        {
            usart0.transmitByte(c);
            _delay_ms(100);
        }
        usart0.transmitByte('\r');
        usart0.transmitByte('\n');


        //Test non-blocking receive-method:
        //If a byte has been received, a message is sent back
        int16_t receivedByte = usart0.receiveByteNonBlocking();
        if (receivedByte != -1)
        {
            usart0.usartPrintf("Received character: %c\r\n", (char) receivedByte);
        }

        //Test of usartScanf()
        int number;
        usart0.usartPrintf("Enter an integer number: ");
        usart0.usartScanf("%d", &number);
        usart0.usartPrintf("The double of %d is %d.\r\n", number, number*2);

        _delay_ms(1000);
    }

    return 0;
}
