/*
    Timer16Bit.cpp - A Module for 16-Bit-Timer/Coutners of AVR-Microcontrollers.

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

#include <stdint.h>
#include <avr/io.h>

#include "Timer16Bit.h"


void TimerCounter16Bit::setMode( Timer16_mode mode )
{
    //Set the Timer/Counter-mode with the WGMn[3..0]-Bits
    //The four WGM-Bits are split: WGMn3 and WGMn2 are in TCCRnB, and WGMn1 and WGMn0 are in TCCRnA
    *m_tccrnb &= ~( (1<<WGM13) | (1<<WGM12) );
    *m_tccrnb |= ( ( ( ((uint8_t) mode) & 0x0C) >> 2 ) << WGM12 );
    *m_tccrna &= ~( (1<<WGM11) | (1<<WGM10) );
    *m_tccrna |=  ( ( ((uint8_t) mode) & 0x03) << WGM10);

}


void TimerCounter16Bit::selectClockSource( Timer16_ClockSource clkSource)
{
    //Set the Timer/Counter-Clock-Source with the CSn[2..0]-Bits
    *m_tccrnb &= ~( (1<<CS12)|(1<<CS11)|(1<<CS10) );
    *m_tccrnb |= ( ((uint8_t)clkSource) << CS10 );
}


int8_t TimerCounter16Bit::setTopValue( uint16_t topCountValue )
{
    switch ( getMode() ) {

        case T16_CTC_OCRXA:
        case T16_PWM_PHI_F_CORRECT_OCRXA:
        case T16_PWM_PHI_CORRECT_OCRXA:
        case T16_FAST_PWM_OCRXA:
            *m_ocrna = topCountValue;
            return 0;

        case T16_PWM_PHI_F_CORRECT_ICRX:
        case T16_PWM_PHI_CORRECT_ICRX:
        case T16_CTC_ICRN:
        case T16_FAST_PWM_ICRX:
            *m_icrn = topCountValue;
            return 0;

        default:
            break;
    }

    return -1;
}


uint16_t TimerCounter16Bit::getTopValue()
{
    switch ( getMode() ) {
        case T16_CTC_OCRXA:
        case T16_PWM_PHI_F_CORRECT_OCRXA:
        case T16_PWM_PHI_CORRECT_OCRXA:
        case T16_FAST_PWM_OCRXA:
            return *m_ocrna;

        case T16_PWM_PHI_F_CORRECT_ICRX:
        case T16_PWM_PHI_CORRECT_ICRX:
        case T16_CTC_ICRN:
        case T16_FAST_PWM_ICRX:
            return *m_icrn;

        case T16_PWM_PHI_CORRECT_0XFF:
        case T16_FAST_PWM_0xFF:
            return 0x00FF;

        case T16_PWM_PHI_CORRECT_0X1FF:
        case T16_FAST_PWM_0X1FF:
            return 0x01FF;

        case T16_PWM_PHI_CORRECT_0X3FF:
        case T16_FAST_PWM_0X3FF:
            return 0x03FF;

        default: //the other valid mode (normal mode) has 0xFFFF as its top-value
            break;
    }
    return 0xFFFF;
}


void TimerCounter16Bit::setCompareMatchValue( Timer16_CompChannel channel, uint16_t compareMatchvalue )
{
    switch (channel) {
        case T16_COMP_A:
            *m_ocrna = compareMatchvalue;
            return;

        case T16_COMP_B:
            *m_ocrnb = compareMatchvalue;
            return;

        #ifdef OCR1C
        case T16_COMP_C:
            *m_ocrnc = compareMatchvalue;
            return;
        #endif

        default:
            break;
    }
}


uint16_t TimerCounter16Bit::getCompareMatchValue( Timer16_CompChannel channel )
{
    switch (channel) {
        case T16_COMP_A:
            return *m_ocrna;

        case T16_COMP_B:
            return *m_ocrnb;;

        #ifdef OCR1C
        case T16_COMP_C:
            return *m_ocrnc;
        #endif

        default:
            break;
    }
    return 0xFFFF; //should never get here
}


void TimerCounter16Bit::setPwmPinMode ( Timer16_CompChannel channel, Timer16_PwmPinMode pwmPinMode)
{

    uint8_t bitOffset = 0;    //position of the two COMnX[1..0] Bits in the TCCRnA-Register

    switch (channel) {

    case T16_COMP_A:
        bitOffset = COM1A0;
        break;

    case T16_COMP_B:
        bitOffset = COM1B0;
        break;

    #ifdef OCR1C
    case T16_COMP_C:
        bitOffset = COM1C0;
        break;
    #endif

    default:
        break; //should never get here
    }


    switch (pwmPinMode)
    {

        case T16_PIN_OFF:
            *m_tccrna &= ~(0x03<<bitOffset);
            return;

        case T16_PIN_TOGGLE_ON_MATCH:
            *m_tccrna &= ~(0x03<<bitOffset);
            *m_tccrna |= (0x01<<bitOffset);
            return;

        case T16_PIN_CLEAR_ON_MATCH:
        case T16_PIN_PWM_NORMAL:
            *m_tccrna &= ~(0x03<<bitOffset);
            *m_tccrna |= (0x02<<bitOffset);
            return;

        case T16_PIN_SET_ON_MATCH:
        case T16_PIN_PWM_INVERTED:
            *m_tccrna |= (0x03<<bitOffset);
            return;

        default:
            break;
    }

    return;
}



void TimerCounter16Bit::forceOutputCompareMatch( Timer16_CompChannel channels)
{
    uint8_t tccrncValue = 0;

    if (channels & T16_COMP_A)    tccrncValue |= (1<<FOC1A);
    if (channels & T16_COMP_B)    tccrncValue |= (1<<FOC1B);
    if (channels & T16_COMP_C)    tccrncValue |= (1<<FOC1C);

    *m_tccrnc = tccrncValue;
}
