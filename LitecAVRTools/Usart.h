/*
    Usart.h - A module for Universal Synchronous Asynchronous
    Receiver Transmitter (USART, RS232-interfaces) of AVR-Microcontrollers

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

#ifndef Usart_h
#define Usart_h

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>


#include "GpioPinMacros.h"    //only needed for sfr8Ptr and sfr16ptr

/*!
 * With this macro, you can choose between Echo and no Echo form the Atmega. If `USE_ECHO` is non-zero, when using
 * `Usart_usartScanf` the ATmega transmits every received char back to the communication partner. This is
 * useful for using a terminal-program (for example putty): While the user types in characters in the
 * terminal program, the echoed back characters are displayed by the terminal-program.
 *
 * If the ATmega shall not echo received characters back to the sender, define `USE_ECHO` as 0.
 */
#define  USE_ECHO   0

/*!
 * When using `Usart::usartPrintf` a Line-Feed-character (LF, '\n', Ascii-Code 0x0A) can be replaced by
 * the two-character sequence Carriage-Return + Line-Feed (CRLF, '\r'+'\n', Ascii-Codes 0x0D, 0x0A).
 * To replace '\n' by "\r\n", define `REPLACE_LF_BY_CRLF` to a non-zero value.
 *
 * If `REPLACE_LF_BY_CRLF` is defined to 0, '\n' is just sent as '\n'. (When using a terminal-program like putty, the
 * user must put a "\r\n"-sequence in the format-string for `Usart:usartPrintf` instead of just only a '\n').
 */
#define REPLACE_LF_BY_CRLF    0

//private functions for stream-IO used by vfprintf and vfscanf
int _usartPut( char c, FILE* stream );
int _usartGet( FILE* stream );

/*!
 * A USART-Error-state returned by Usart::getErrorState() is the bitwise OR the constants of this emum
 */
enum UsartErrors
{
    cUsart_ParityError = (1<<2),
    cUsart_DataOverrunError = (1<<3),
    cUsart_FrameError = (1<<4)
};

//enum UsartConfiguration copied from Igor Mikolic-Torreira

/*!
 * \brief This enum lists USART configuration in terms of data bits, parity, and stop bits.
 *
 * The format is cUsart_XYZ where
 * - X = the number of data bits
 * - Y = N, E, or O; where N = none, E = even, and O = odd
 * - Z = the number of stop bits
 *
 * \hideinitializer
 */
enum UsartConfiguration
{
    cUsart_5N1 = 0x00,     //!< 5 data bits, no parity, 1 stop bit  \hideinitializer
    cUsart_6N1 = 0x02,     //!< 6 data bits, no parity, 1 stop bit  \hideinitializer
    cUsart_7N1 = 0x04,     //!< 7 data bits, no parity, 1 stop bit  \hideinitializer
    cUsart_8N1 = 0x06,     //!< 8 data bits, no parity, 1 stop bit  \hideinitializer
    cUsart_5N2 = 0x08,     //!< 5 data bits, no parity, 2 stop bits  \hideinitializer
    cUsart_6N2 = 0x0A,     //!< 6 data bits, no parity, 2 stop bits  \hideinitializer
    cUsart_7N2 = 0x0C,     //!< 7 data bits, no parity, 2 stop bits  \hideinitializer
    cUsart_8N2 = 0x0E,     //!< 8 data bits, no parity, 2 stop bits  \hideinitializer
    cUsart_5E1 = 0x20,     //!< 5 data bits, even parity, 1 stop bit  \hideinitializer
    cUsart_6E1 = 0x22,     //!< 6 data bits, even parity, 1 stop bit  \hideinitializer
    cUsart_7E1 = 0x24,     //!< 7 data bits, even parity, 1 stop bit  \hideinitializer
    cUsart_8E1 = 0x26,     //!< 8 data bits, even parity, 1 stop bit  \hideinitializer
    cUsart_5E2 = 0x28,     //!< 5 data bits, even parity, 2 stop bits  \hideinitializer
    cUsart_6E2 = 0x2A,     //!< 6 data bits, even parity, 2 stop bits  \hideinitializer
    cUsart_7E2 = 0x2C,     //!< 7 data bits, even parity, 2 stop bits  \hideinitializer
    cUsart_8E2 = 0x2E,     //!< 8 data bits, even parity, 2 stop bits  \hideinitializer
    cUsart_5O1 = 0x30,     //!< 5 data bits, odd parity, 1 stop bit  \hideinitializer
    cUsart_6O1 = 0x32,     //!< 6 data bits, odd parity, 1 stop bit  \hideinitializer
    cUsart_7O1 = 0x34,     //!< 7 data bits, odd parity, 1 stop bit  \hideinitializer
    cUsart_8O1 = 0x36,     //!< 8 data bits, odd parity, 1 stop bit  \hideinitializer
    cUsart_5O2 = 0x38,     //!< 5 data bits, odd parity, 2 stop bits  \hideinitializer
    cUsart_6O2 = 0x3A,     //!< 6 data bits, odd parity, 2 stop bits  \hideinitializer
    cUsart_7O2 = 0x3C,     //!< 7 data bits, odd parity, 2 stop bits  \hideinitializer
    cUsart_8O2 = 0x3E      //!< 8 data bits, odd parity, 2 stop bits  \hideinitializer
};

/*!
 * A class for USARTs of AVR-microcontrollers. The ATmega328p has only one USART, the ATmega2560 has four
 */
class Usart {
public:

    /*!
     * Constructor. Use the `makeUsartObject`-macro to create an object of this class.
     * For example: Create an Object for USART2 of the ATmega2560:
     * ```C
     * Usart myUsart = makeUsartObject( 2 );
     * ```
     */
    Usart( sfr8Ptr ucsra, sfr8Ptr ucsrb, sfr8Ptr ucsrc,
            sfr16Ptr ubrr, sfr8Ptr udr )
        : m_ucsra(ucsra)
        , m_ucsrb(ucsrb)
        , m_ucsrc(ucsrc)
        , m_ubrr(ubrr)
        , m_udr(udr)
        {
            //see the comments in stdio.h and fdevopen.c from avrlibc:
            //fdev_setup_stream initializes the (private) FILE-struct m_stream of this Usart-Object
            fdev_setup_stream( &m_stream, s_usartPut, s_usartGet,_FDEV_SETUP_RW );
            //A FILE-struct can contain user-data (a pointer to void). Pass
            //this Usart-Object as user-data to the FILE-struct.
            fdev_set_udata( &m_stream, (void*) this );
            //The m_stream FILE-struct is passed by the methods Usart::usartPrintf() and Usart::usartScanf() to the
            //Functions vfprintf() and vfscanf() from stdio.h. These two functions then use s_usartPut() and
            //s_usartGet() to put and get characters to/from the "stream". Because the s_usartPut()- and
            //s_usartGet()-functions also receive the FILE-struct m_stream as an argument, they can fetch its
            //user-data, which is Pointer to this Usart-Object. Then they call from this object the methods
            //transmitByte() and receiveByte().
            //...it's not so complicated as it seems to be.
        }

    /*!
     * Initializes the Usart. This is the first method to call, before calling any other method.
     *
     * \arg \c baudrate Choose one of the normally used baudrates, like 9600, 115200, ...
     * \arg \c config Use one of the `cUsart_xYZ`-constants from enum `UsartConfiguration` to define number of
     *         data-bits, parity and number of stop-bits
     */
    void init( uint32_t baudrate, uint8_t config = cUsart_8N1 );

    /*!
     * Enables or disables the three Interrupts of a USART:
     *
     * \arg \c rxComplete Pass 1 to enable the Receive-Complete-Interrupt, and 0 to disable it.
     * \arg \c txComplete Pass 1 to enable the Transmit-Complete-Interrupt, and 0 to disable it.
     * \arg \c udrEmpty Pass 1 to enable the Data-Register-Empty-Interrupt, and 0 to disable it. This Interrupt-Event
     *         happens, when the transmitter is still transmitting a byte, but its data-register (UDR0) can be loaded
     *         with a new byte. This byte is immediately transmitted, when the current byte finishes transmitting.
     */
    void enableOrDisableInterrupts( uint8_t rxComplete, uint8_t txComplete, uint8_t udrEmpty )
    {
        *m_ucsrb &= ~(rxComplete<<RXCIE0) & ~(txComplete<<TXCIE0) & ~(udrEmpty<<UDRIE0);
        *m_ucsrb |= ((rxComplete<<RXCIE0)|(txComplete<<TXCIE0)|(udrEmpty<<UDRIE0));
    }

    /*!
     * Returns The error-state of the USART-Receiver. If errors are evaluated, the error-state must
     * be read before the received byte is read, because reading a received byte clears the error-state.
     *
     * \returns 0x00, if no error has happened, or a bitwise "or" of  `cUsart_ParityError`, `cUsart_DataOverrunError`
     *          and/or `cUsart_FrameError`, depending on the errors happened, whil receiving a byte.
     */
    uint8_t getReceiveErrors()
    {
        return *m_ucsra & (cUsart_ParityError|cUsart_DataOverrunError|cUsart_FrameError);
    }

    /*!
     * Waits until the transmit-buffer is ready to receive a new byte to transmit, then write `c` to the
     * transmit-buffer. The USART-Hardware will then start to transmit the byte
     *
     * \arg \c c The byte (character to be transmitted)
     */
    void transmitByte( uint8_t c );
    void transmitByte( char c ) { transmitByte( (uint8_t) c); }

    /*!
     * If the transmit-buffer is ready to receive a new byte to transmit, then write `c` to the transmit-buffer.
     * If the transmit-buffer is not ready, return without transmitting anything immediately.
     *
     * \arg \c c The byte (character to be transmitted)
     * \returns 0, if `c` has succesfully been queued for transmission, and -1 otherwise.
     */
    int8_t transmitByteNonBlocking( uint8_t c );
    int8_t transmitByteNonBlocking( char c ) { return transmitByteNonBlocking( (uint8_t) c ); }

    /*!
     * Waits until a byte received by the USART is in the receive-buffer, and returns this byte.
     *
     * \returns the byte received. If you get warnings, mayby casting the returned value to `(char)` helps.
     */
    uint8_t receiveByte();

    /*!
     * Returns the last received byte by the USART (which is stored in the receive-buffer of the AVR). If no byte has
     * been received yet, this mehtod returns immediately.
     *
     * \returns -1, if no byte has been received yet. Otherwise returns the received byte cast to an int16_t.
     */
    int16_t receiveByteNonBlocking();

    /*!
     * Returns 0, if no byte has been received. Non-zero is returned, if a byte has been received, and not yet fetched 
     * by the `receiveByte()` or `receiveByteNonBlocking()`-method.
     */
     uint8_t byteAvailable()
     { return *m_ucsra & (1<<RXC0); }

    /*!
     * printf() for the usart. Use it exactly as you would use printf. For example:
     * ```C
     * int i = 42;
     * Usart usart0 = makeUsartObject( 0 );
     * usart0.init(9600);
     * usart0.usartPrintf("The Answer to the Ultimate Question of Life, The Universe, and Everything: %d\r\n", i ); //Douglas Adams
     * ```
     */
    int usartPrintf(const char* fmt, ... );

    /*!
     * scanf() for the usart. Use it exactly as you would use scanf. For example:
     * ```C
     * int i;
     * Usart usart0 = makeUsartObject( 0 );
     * usart0.init(9600);
     * usart0.usartPrintf("The Answer to the Ultimate Question of Life, The Universe, and Everything: %d\r\n", i ); //Douglas Adams
     * ```
     */
    int usartScanf(const char* fmt, ...);

private:

    //Only for internal use (callback-Functions for FILE-struct used by vfprintf() and vfscanf()
    static int s_usartPut( char c, FILE* stream );
    static int s_usartGet( FILE* stream );

    sfr8Ptr  m_ucsra;
    sfr8Ptr  m_ucsrb;
    sfr8Ptr  m_ucsrc;
    sfr16Ptr m_ubrr;
    sfr8Ptr  m_udr;
    FILE     m_stream; //not just a pointer, but a real FILE-struct
};


/*!
 * Use this macro to initialize a `Usart`-Object.
 *
 * \arg \c usartNo is the Number of the USART. For the Atmega2560 this is 0..3, for the Atmega328p `usartNo` must be 0.
 */
#define makeUsartObject( usartNo )          Usart( &UCSR##usartNo##A, &UCSR##usartNo##B, &UCSR##usartNo##C, &UBRR##usartNo, &UDR##usartNo )



#endif
