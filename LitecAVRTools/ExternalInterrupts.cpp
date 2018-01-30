/*
    ExternalInterrupts.cpp - A module for Interrupts caused by
    voltage-level-changes on the INTx-Pins of AVR-Microcontrollers

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

#include <avr/io.h>

#include "ExternalInterrupts.h"


// Number of external Interrupts
#if defined(INT7_vect)
    #define EXT_INT_COUNT  8
#elif defined(INT6_vect)
    #define EXT_INT_COUNT  7
#elif defined(INT5_vect)
    #define EXT_INT_COUNT  6
#elif defined(INT4_vect)
    #define EXT_INT_COUNT  5
#elif defined(INT3_vect)
    #define EXT_INT_COUNT  4
#elif defined(INT2_vect)
    #define EXT_INT_COUNT  3
#elif defined(INT1_vect)
    #define EXT_INT_COUNT  2
#elif defined(INT0_vect)
    #define EXT_INT_COUNT  1
#else
    #error "There are no external Interrupts. Don't use this module"
#endif

//////////////////////////////////////////////////////////////////////////
// C-Functions-API
//////////////////////////////////////////////////////////////////////////

void setExtIntEventType( uint8_t extIntNumber, uint8_t extIntEventType )
{
	//Check, if the given external interrupt exists on this microcontroller:
    if (extIntNumber >= EXT_INT_COUNT) return;
    
    if ( extIntEventType > 0x03 ) return;
    
    if (extIntNumber < 4)
    {
        //first clear Bits ISCn1 and ISCn0 in the EICRA-Register (n is the
        //number of the external Interrupt. Then set these Bits to:
        //  00 ... for low level active Interrupts
        //  01 ... for "any edge" (both edges causing Interrupt-Events)
        //  10 ... for falling edges causing Interrupt-Events
        //  11 ... for rising edges causing Interrupt-Events
        EICRA &= ~(0x03 << extIntNumber*2);  //clear bits ISCn[1..0]
        EICRA |= (extIntEventType << extIntNumber*2); //then write Bits
        return;
    }
    
    //for the ATmega2560 the ISCn[1..0]-Bits for the external Interrupts 4 to 7
    //are in special-function-register EICRB
    #ifdef EICRB
    if (extIntNumber <8)
    {
        extIntNumber -= 4; 
        EICRB &= ~(0x03 << extIntNumber*2); //clear bits ISCn[1..0]
        EICRB |= (extIntEventType << extIntNumber*2); //then set these Bits
        return;
    }
    #endif
}

void enableExtInt( uint8_t extIntNumber )
{
    if (extIntNumber >= EXT_INT_COUNT) return;
    EIMSK |= (0x01<<extIntNumber);
}


void disableExtInt( uint8_t extIntNumber )
{
    if (extIntNumber >= EXT_INT_COUNT) return;
    EIMSK &=~ (0x01<<extIntNumber);
}

void clearPendingExtIntEvent( uint8_t extIntNumber )
{
    if (extIntNumber >= EXT_INT_COUNT) return;   
    //To clear a pending interrupt, must write 1 to the interrupt-flag
    //This is called "write 1 to clear"
    EIFR |= (0x01<<extIntNumber);
}
