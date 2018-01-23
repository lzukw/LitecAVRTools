/*
    ExternalInterrupts.h - A Library for Interrupts caused by 
    voltage-level-changes on the INTx-Pins of AVR-Microcontrollers
    (The so called external Interrupt-Pins).
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

#ifndef EXTERNALINTERRUPTS_H_
#define EXTERNALINTERRUPTS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
// Macros used as arguments for function-/method-calls
//////////////////////////////////////////////////////////////////////////

/**
 * These Macros are used as argument for the parameter `extIntEventType`
 * of the function `setExtIntEventType`, the constructor of the `ExtInt`-class,
 * and the method `ExtInt::setExtIntEventType`.
 */
#define EXTINT_LOW_LEVEL_ACTIVE         0x00
#define EXTINT_ANY_EDGE                 0x01
#define EXTINT_FALLING_EDGE             0x02
#define EXTINT_RISING_EDGE              0x03  


//////////////////////////////////////////////////////////////////////////
// C-Function-API
//////////////////////////////////////////////////////////////////////////

/**
 * This function defines, which voltage-change-Events on an INTx-Pin actually
 * cause an Interrupt-Event. 
 *
 * @param extIntNumber The Number of the external Interrupt. Between 0 and up
 * 		to 7, depending on the used AVR-microcontroller
 * @param extIntEventType Use one of the following:
 *      - EXTINT_LOW_LEVEL_ACTIVE (Macro with value 0x00). A low-level on Pin
 *        INTx produces ongoing interrupt-events (mostly used with external
 *        chips, that have more than one internal Interrupt-Source, but only one
 *        Interrupt-output.
 *      - EXTINT_ANY_EDGE (Macro with value 0x01). Both, rising and falling
 *        edges (voltage-changes from high- to low-level and vice versa) shall
 *        cause an Interrupt-Event.
 *      - EXTINT_FALLING_EDGE (Macro with value 0x02). Only the falling edges
 *        (voltage-changes from high- to low-level) shall cause an
 *        Interrupt-Event.
 *      - EXTINT_RISING_EDGE (Macro with value 0x03). Only rising edges
 *        (voltage-changes from low- to high-level) shall cause an
 *        Interrupt-Event
 */
void setExtIntEventType( uint8_t extIntNumber, uint8_t extIntEventType );


/**
 * Enables an external Interrupt. If Interrupts are also globally allowed (for
 * example using `sei();` from <avr/interrupt.h>), then the
 * Interrupt-Service-Routine is executed, each time an external 
 * Interrupt-event happens.
 *
 * You have to implement the Interrupt-Service-Routine using the ISR-Macro
 * from <avr/interrupt.h> with the correct Interrupt-vector (`INTx_vect`).
 *
 * @param extIntNumber The Number of the external Interrupt
 */
void enableExtInt( uint8_t extIntNumber );

/**
 * Disables an external Interrupt. The internal Interrupt-Service-Routine is
 * not executed any longer, if an Interrupt-Event happens.
 *
 * @param extIntNumber The Number of the external Interrupt
 */
void disableExtInt( uint8_t extIntNumber );

/**
 * If an Interrupt-Event happens, while the corresponding external Interrupt
 * is disabled, an internal Interrupt-Flag in the CPU is set. This flag stores
 * the information for the CPU, that an Interrupt-Event has happened. Normally
 * it is cleared automatically, when the Interrupt-Service-Routine is executed.
 *
 * If later the external Interrupt is enabled, the Interrupt-Service-Routine is
 * executed immediately, because the Interrupt-Flag is set. If you want to
 * prevent this, call `clearPendingInterruptEvent` before enabling the external
 * Interrupt.
 *
 * @param extIntNumber The Number of the external Interrupt
 */
void clearPendingExtIntEvent( uint8_t extIntNumber );

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

//////////////////////////////////////////////////////////////////////////
// C++ object-oriented API
//////////////////////////////////////////////////////////////////////////

/**
 * Lightwight class for external Interrupts. Use one ExtInt-Instance for each
 * external interrupt used in your program.
 */
class ExtInt
{
public:
    /**
     * Constructor.
     *
     * @param extIntNumber The Number of the external Interrupt
     * @param extIntEventType Use one of the Macros EXTINT_LOW_LEVEL_ACTIVE,
     *      EXTINT_ANY_EDGE, EXTINT_FALLING_EDGE or EXTINT_RISING_EDGE.
     *      see C-function `setExtIntEventType`.
     */
    ExtInt( uint8_t extIntNumber, uint8_t extIntEventType )
            : mExtIntNumber(extIntNumber)
    { ::setExtIntEventType( mExtIntNumber, extIntEventType );  }
    
    /**
     * This method defines, which voltage-change-Events on an INTx-Pin actually
     * cause an Interrupt-Event. Normally you define this already with the
     * parameterized constructor of the `ExtInt`-Instance. But in some
     * cases you want to toggle between rising and falling edges causing an
     * interrupt. You can switch this even within the Interrupt-Service-Routine.
     *
     * @param extIntEventType Use one of the Macros EXTINT_LOW_LEVEL_ACTIVE,
     *      EXTINT_ANY_EDGE, EXTINT_FALLING_EDGE or EXTINT_RISING_EDGE.
     *		see C-function `setExtIntEventType`.
     */
    void setExtIntEventType( uint8_t extIntEventType ) 
    { ::setExtIntEventType( mExtIntNumber, extIntEventType ); }

    /**
	 * Enables an external Interrupt. If Interrupts are also globally allowed
	 * (for example using `sei();` from <avr/interrupt.h>), then the
	 * Interrupt-Service-Routine is executed, each time an external
	 * Interrupt-event happens.
	 *
	 * You have to implement the Interrupt-Service-Routine using the
	 * ISR-Macro from <avr/interrupt.h> with the correct
	 * Interrupt-vector (`INTx_vect`).
     */
    void enable()
    { ::enableExtInt( mExtIntNumber ); }

    /**
     * Disables an external Interrupt. The internal Interrupt-Service-Routine
     * is not executed any longer, if an Interrupt-Event happens.
     */
    void disable()
    { ::disableExtInt(mExtIntNumber); }

    /**
     * Clears pending Interrupts (that occur, while Interrupts are
     * disabled).See C-function `clearPendingInterruptEvent` for more
     * explanation on how pending interrupt-events arise.
     */
    void clearPendingEvent()
    {
        ::clearPendingExtIntEvent(mExtIntNumber);
    }

private:
    uint8_t mExtIntNumber;
};

#endif


#endif /* EXTERNALINTERRUPTS_H_ */
