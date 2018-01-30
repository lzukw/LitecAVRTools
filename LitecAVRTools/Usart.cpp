/*
    Usart.cpp - A module for Universal Synchronous Asynchronous
    Receiver Transmitter (USART, RS232-interfaces) of AVR-Microcontrollers

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

#include <stdarg.h>

#include "Usart.h"

#ifndef F_CPU
    #error "F_CPU not defined. Must be set for calculating baudrate in Usart.cpp"
#endif

//private functions for stream-IO used by vfprintf and vfscanf.
//The user-data of the FILE-struct contains a pointer to the Usart-Object, whose methods called
//vfprintf() or vfscanf(). These two functions from stdio.h call the FILE-structs put- and get-functions, which
//point to _usartPut() and _usartGet(). Here we retreive the user-data from the FILE-struct, convert it back to
//a pointer to a Usart-Object, and call its tramsitByte() or receiveByte()-method.

int Usart::s_usartPut( char c, FILE* stream )
{
#if (REPLACE_LF_BY_CRLF != 0)
    static char lastSentChar;
    if (c == '\n' && lastSentChar != '\r')
        Usart::s_usartPut('\r', stream);
    lastSentChar = c;
#endif

    Usart* usart = (Usart*) fdev_get_udata(stream);
    usart->transmitByte( c );
    return 0;
}

int Usart::s_usartGet( FILE* stream )
{
    Usart* usart = (Usart*) fdev_get_udata(stream);
    uint8_t c = usart->receiveByte();
    #if (USE_ECHO != 0)
    usart->transmitByte( c );
    #endif
    return (int) c;
}




void Usart::init(  uint32_t baudrate, uint8_t config )
{
    *m_ucsra = (1<<U2X0); //use double speed, don't use multiprocessor communication mode
    *m_ucsrb = (1<<RXEN0)|(1<<TXEN0); //emable receiver and transmitter, UCSZ2n=0 (frames with 9 data-bits not used)
    *m_ucsrc = config; // asynchronous Mode, data-bits/parity/stop-bits set by config

    //we use double speed, because baudrates have less tolerance.
    *m_ubrr = (uint16_t) ( F_CPU / (baudrate * 8) - 1);

    //Integer division always rounds down. Increase UBRRn-Register, if rounding
    //up would deliver a better value
    if ( F_CPU % (baudrate*8) > baudrate*8/2 )
        (*m_ubrr)++;
}


void Usart::transmitByte( uint8_t c )
{
    //wait until transmit-buffer is ready to be loaded with new byte to
    //transmit
    while( ! (*m_ucsra & (1<<UDRE0)) ) { /*empty*/ }
    *m_udr = c;
}

int8_t Usart::transmitByteNonBlocking( uint8_t c )
{
    //wait until transmit-buffer is ready to be loaded with new byte to
    //transmit
    if ( ! (*m_ucsra & (1<<UDRE0)) ) return -1;
    *m_udr = c;
    return 0;
}

uint8_t Usart::receiveByte()
{
    while( ! (*m_ucsra & (1<<RXC0)) ) { /*empty*/ }
    return *m_udr;
}

int16_t Usart::receiveByteNonBlocking()
{
    if ( ! (*m_ucsra & (1<<RXC0)) ) return -1;
    return (int16_t) *m_udr;
}

int Usart::usartPrintf(const char* fmt, ... )
{
    va_list args;
    va_start(args, fmt);
    int retval = vfprintf( &m_stream, fmt, args);
    va_end(args);

    return retval;
}

int Usart::usartScanf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int retval = vfscanf( &m_stream, fmt, args );
    va_end(args);

    return retval;
}
