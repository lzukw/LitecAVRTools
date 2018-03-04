/*
    Timer16Bit.h - A Module for 16-Bit-Timer/Coutners of AVR-Microcontrollers.

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

#ifndef TIMER_16_BIT_H_
#define TIMER_16_BIT_H_

#include <stdint.h>
#include <stddef.h>
#include <avr/io.h>

#include "GpioPinMacros.h"    //only needed for sfr8Ptr and sfr16ptr


//////////////////////////////////////////////////////////////////////////
// Macros used as parameters for functions/methods
//////////////////////////////////////////////////////////////////////////

/*!
 * Operating-modes for Timer/Counters. These modes define, how the Timer/Counter counts its internal
 * Count-Value (TCNTx-Register) up and down.
 * The hex-value represents the WGMn[3..0]-Bits in the TCCRxA and TCCRxB-Registers
 */
enum Timer16_mode
{
    T16_NORMAL =                       0x00,
    T16_PWM_PHI_CORRECT_0XFF =         0x01,
    T16_PWM_PHI_CORRECT_0X1FF =        0x02,
    T16_PWM_PHI_CORRECT_0X3FF =        0x03,
    T16_CTC_OCRXA =                    0x04,
    T16_FAST_PWM_0xFF =                0x05,
    T16_FAST_PWM_0X1FF =               0x06,
    T16_FAST_PWM_0X3FF =               0x07,
    T16_PWM_PHI_F_CORRECT_ICRX =       0x08,
    T16_PWM_PHI_F_CORRECT_OCRXA =      0x09,
    T16_PWM_PHI_CORRECT_ICRX =         0x0A,
    T16_PWM_PHI_CORRECT_OCRXA =        0x0B,
    T16_CTC_ICRN =                     0x0C,
    //Timer Mode 0x0D is reserved
    T16_FAST_PWM_ICRX =                0x0E,
    T16_FAST_PWM_OCRXA =               0x0F
};

/*!
 * Clock-select-Values for the 16-Bit-Timer/Counters. The Timer/Counter can either be clocked by the
 * CPU-oscillator-clock signal (16MHz on most Arduino-Boards), or by voltage-level-changes on the TCx-input-pin
 * of the Timer/Counter.
 *
 * When using the internal clock signal (Timer-mode), the oscillator-frequency f_osc can be used directly, or one of
 * four prescaled frequencies can be used: f_osc/8, f_osc/64, f_osc/256 and f_osc/1024.
 * 
 * When using voltage-level-changes on the TCx-input-pin belonging to the Timer/Counter as clock-source (Counter-mode),
 * either rising edges (low-to-high voltage-level changes) or falling edges (high-to-low transitions) can be selected.
 *
 * `T16_CLK_OFF` means, that the timer is stopped (not counting).
 *
 * The hex-value represents the CSn[2..0]-Bits in the TCCRnB-Register
 */
enum Timer16_ClockSource
{
    T16_CLK_OFF =          0x00,
    T16_PRESC_1 =          0x01,
    T16_PRESC_8 =          0x02,
    T16_PRESC_64 =         0x03,
    T16_PRESC_256 =        0x04,
    T16_PRESC_1024 =       0x05,
    T16_FALLING =          0x06,
    T16_RISING =           0x07
};

/*!
 * There are two (ATmega328p) or three (ATmega2560) Compare-Match-Registers. Each Compare-Match-Register can have a
 * PWM-output-Pin associated with it. For example Timer/Counter1 on the ATmega2560 has the Compare-Match-Registers
 * `OCR1A`, `OCR1B`, `OCR1C`, with the Pins OC1A, OC1B and OC1C (which are alternative Functions for the GPIO-Pins PB5,
 * PB6, PB7). To refer to one of the three Compare-Match-Registers, as well as to refer to one of the three PWM-Pins,
 * the constants of this enum are used.
 *
 * The constants of this enum can be or'ed when referring to more than one channel.
 */
enum Timer16_CompChannel
{
    T16_COMP_A =       0x01,
    T16_COMP_B =       0x02,
    #ifdef OCR1C
    T16_COMP_C =       0x04
    #endif
};

//To be able to bitwise or Timer16_CompChannel-Constants
inline Timer16_CompChannel operator|(Timer16_CompChannel a, Timer16_CompChannel b)
{ return static_cast<Timer16_CompChannel>( static_cast<uint8_t>(a) | static_cast<uint8_t>(b) ); }


/*!
 * Each 16-Bit-Timer has two (ATmega328p) or three (Atmega2560) output-pins to generate a Hardware-PWM-Signal.
 * The output-pins are named OCnA, OCnB, and OCnC. (n is the Number of the Timer/Counter).
 *
 * For normal GPIO-pin-operation (not using PWM), use `T16_PIN_OFF`.
 *
 * For PWM-Modes, typically `T16_PIN_PWM_NORMAL`  or `T16_PIN_PWM_INVERTED` are used. (Only for the OCnA-pin
 * `T16_PIN_TOGGLE_ON_MATCH` can be used).
 *
 * For normal and CTC-Mode typically `T16_PIN_OFF` is used, so no Hardware-PWM is performed and the Pins
 * work as normal GPIO-Pins. For creating a 50%-duty-cycle Signal with varying frequency typically on the OCnA-Pin
 * `T16_PIN_TOGGLE_ON_MATCH` is used.
 *
 * `T16_PIN_CLEAR_ON_MATCH` and `T16_PIN_SET_ON_MATCH` are used in non-PWM-modes, typically only to initialize the
 * state of the OCnX-Pin using `forceOutputCompareMatch` prior to converting the pin to an output and switching to
 * PWM-mode.
 */
enum Timer16_PwmPinMode
{
    T16_PIN_OFF =              0,
    T16_PIN_TOGGLE_ON_MATCH =  1,
    T16_PIN_CLEAR_ON_MATCH =   2,
    T16_PIN_SET_ON_MATCH =     3,
    T16_PIN_PWM_NORMAL =       4,
    T16_PIN_PWM_INVERTED =     5
};

/*!
 * Each 16-Bit-Timer/Counter has four (ATmega328p) or five (ATmega2560) Interrupt-Event-Sources, which can individually
 * be enabled or disabled
 *
 * The constants of this enum can be or'ed when enabling or disabling more than one interrupt.
 */
enum Timer16_Interrupts
{
    T16_INT_OVERFLOW =         0x01,
    T16_INT_COMP_MATCH_A =     0x02,
    T16_INT_COMP_MATCH_B =     0x04,
    T16_INT_COMP_MATCH_C =     0x08,
    T16_INT_INPUT_CAPT =       0x20
};

//To be able to bitwise or Timer16_InterruptEvent-Constants
inline Timer16_Interrupts operator|(Timer16_Interrupts a, Timer16_Interrupts b)
{ return static_cast<Timer16_Interrupts>( static_cast<uint8_t>(a) | static_cast<uint8_t>(b) ); }


//TODO: Add defines and functions for input-capture

//////////////////////////////////////////////////////////////////////////
// C++ class API
//////////////////////////////////////////////////////////////////////////

class TimerCounter16Bit
{
public:

    /*!
     * Constructor. Use the `makeTimerCounter16BitObject`-macro to create an object of this class.
     * For example: Create an Object for Timer/Counter 1
     * ```C
     * TimerCounter16Bit myTimer = makeTimerCounter16BitObject( 1 );
     * ```
     */
    TimerCounter16Bit(sfr8Ptr tccrna , sfr8Ptr tccrnb, sfr8Ptr tccrnc, sfr16Ptr tcntn, sfr16Ptr ocrna,
            sfr16Ptr ocrnb, sfr16Ptr ocrnc, sfr16Ptr icrn, sfr8Ptr timsk, sfr8Ptr tifr )
            : m_tccrna(tccrna)
            , m_tccrnb(tccrnb)
            , m_tccrnc(tccrnc)
            , m_tcntn(tcntn)
            , m_ocrna(ocrna)
            , m_ocrnb(ocrnb)
            , m_ocrnc(ocrnc)
            , m_icrn(icrn)
            , m_timsk(timsk)
            , m_tifr(tifr)
    { /*empy */ }


    /*!
     * Sets the Timer/Counter mode. There are 15 different modes: Normal-mode, CTC-mode, Fast-PWM modes,
     * phase-correct-PWM modes and phase-and-frequency-correct modes.
     *
     * The Timer-mode defines which value is used as TOP-count-value, and if single- or dual-slope counting is used.
     *
     * \arg \c mode The Timer/Counter operating-mode. Pass once of the constants from enum `Timer16_mode`
     */
    void setMode( Timer16_mode mode );

    /*!
     * Returns the actual Timer/Counter operating mode, that was set when calling the `init`-method. The
     * return-value is one of constants from enum `Timer16_mode`.
     */
    Timer16_mode getMode()
    { return (Timer16_mode) ( ( ( *m_tccrna >> WGM10 ) & 0x03 ) | ( ((*m_tccrnb>>WGM12)<<2) & 0x0C ) ); }

    /*!
     * Selects the clock-source. With each clock-pulse the the actual count-value of the Timer/Counter changes by one.
     * In Timer-mode the CPU-oscillator-frequency, divided by a prescaler-Value of 1, 8, 64, 256 or 1024 is used
     * as clock-source. in Counter-mode, rising or falling edges on the TCn-input-Pin of the Timer/Counter are
     * counted.
     *
     * With `T16_CLK_OFF` the clock is turned off, and the count-value stays constant.
     *
     * \arg \c clkSource The clock-source for the Timer/Counter. Use one of the enum-constants
     */
    void selectClockSource( Timer16_ClockSource clkSource );

    /**
     * This method changes the value of the actual count-value of the Timer/Counter (register TCNTx).
     *
     * Normally it is not necessary to use this method. If you think about setting the count-value,
     * then better use another Timer-Mode and `setTopValue()` instead.
     *
     * \arg \c countValue This value is assigned to the Timer/Counter-count-Register (TCNTx) holding the actual count
     *      value.
     */
    void setActualCountValue( uint16_t countValue )
    { *m_tcntn = countValue; }


    /**
     * Returns the actual count-value of the Timer/Counter (content of register TXNTx).
     */
    uint16_t getActualCountValue()
    { return *m_tcntn; }


    /*!
     * Some timer-modes permit setting a TOP-value. This is the maximum value the count-register reaches. After
     * reaching the TOP-value, the count-value is either set to zero (for single-slope-modes, like Fast-PWM or
     * CTC-Modes), or the count-register-value changes from increasing to decreasing with every timer-clock-pulse
     * (dual-slope-modes, like Phase-correct PWM modes).
     *
     * If you use the internal-clock-source for the Timer/Counter, setting the TOP-value changes the
     * frequency of Timer-Interrupts or of the PWM-Signal produced by the Timer/Counter
     *
     * Timer-modes, that permit setting the TOP-Value, use either a fixed top-value, or they use the OCRxA- or the
     * ICRx-Register for storing this value. This function automatically detects the used Timer-mode and writes
     * `topCountValue`to the correct register. Therfore, before using this method, the timer-operating-mode should
     * have been chosen using `setMode`.
     *
     * \arg \c topCountValue Maximum Value the count-register reaches in its counting-sequence.
     *
     * \returns 0 on success (if `topCountValue` was stored in either OCRnA or ICRn), or -1, if the timer-mode
     *      does not permit setting a TOP-value, because it has a fixed TOP-value.
     */
    int8_t setTopValue( uint16_t topCountValue );


    /*!
     * Returns the TOP-Value of the Timer/Counter. Depending on the operating-mode, this is either a fixed value, or
     * the value in the OCRnA- or the ICRn-Register.
     */
    uint16_t getTopValue();


    /*!
     * On an ATmega328p there are two compare-match-registers for generating interrupts or a PWM-Signal:
     * OCR1A and OCR1B. The ATmega2560 has three compare-match-registers for each timer: OCRnA, OCRnB and OCRnC.
     * If the actual count-value of the Timer/Counter is equal to the value in one of these registers, an
     * interrupt-event occurs, and the voltage-level of the corresponding Timer-output-pin (OCnA, OCnB, OCnC) is
     * changed
     *
     * This method writes the value into the according OCnX-register.
     *
     * In some operating-modes, OCRnA is used as TOP-value. In these modes using `T16_COMP_A` for the
     * `channel`-argument this method also changes the TOP-value.
     *
     * \arg \c channel `T16_COMP_A`, `T16_COMP_B` or `T16_COMP_C` (if available).
     *
     * \arg \c compareMatchValue The value written to the compare-match-register (only values lower than the
     *      TOP-value are useful.
     */
    void setCompareMatchValue( Timer16_CompChannel channel, uint16_t compareMatchvalue );


    /*!
     * Returns the value of the given compare-match-register (OCRnA, OCRnB, OCRnC if available).
     *
     * In some operating-modes, OCRnA is used as TOP-value. For these modes, `getCompareMatchValue` returns the same
     * value as ' getTopValue'
     */
    uint16_t getCompareMatchValue( Timer16_CompChannel channel );


    /**
     * Chooses how the PWM-output-Pins associated with the 16-Bit-Timer/Counter behave.
     *
     * ATTENTION: If you want to use a PWM-Pin, you also have to set the according GPIO-Pin as an output, for example
     * using `ssetGpioPinModeOutput()`. If you don't set the Pin as an output, it remains an input, and no PWM-Signal
     * will appear on the Pin. For starting with the correct voltage-level (for example when driving a
     * MOSFET-half-bridge `forceOutputCompareMatch` can be used.
     *
     * \arg \c channel Each Timer/Counter has up to three compare-match-channels. A PWM-Pin is associated with each
     *      channel. The PWM-Pins are called OCnA, OCnB and OCnC in the datasheet (n is the Timer/Counter-number).
     *      Use one of the enum-constants `T16_COMP_A`, `T16_COMP_B` or `T16_COMP_C` (if available) for this parameter.
     *
     * \arg \c pwmPinMode Use one of the enum-constants that begin with `T16_PIN_` for this Parameter. See
     *      the enum `Timer16_PwmPinMode` and the datasheet for more information. It depends on the used timer-mode,
     *      which PWM-pin-modes are useful.
     *
     * \see `Timer16_PwmPinMode`
     */
    void setPwmPinMode( Timer16_CompChannel channel, Timer16_PwmPinMode pwmPinMode);


    /*!
     * If the Timer-output-pins OCnA, OCnB, OCnC are used, they can be initialized to High- or Low-voltage-state by
     * forcing a compare-Match. This must be done before the operating mode is changed to a PWM-Mode, and usually is
     * done before the the OCnX-pins are programmed as outputs. See the "16-Bit-Timer/Counter"-section of the
     * datasheet for more details.
     *
     * \arg \c channels A bitwise 'or' of the enum-constants `T16_COMP_A`, `T16_COMP_B` and `T16_COMP_C` (if
     *      available).
     */
    void forceOutputCompareMatch( Timer16_CompChannel channels);

    /*!
     * Enable one or more of the Interrupts of this 16-Bit-Timer-Counter.
     *
     * To use the interrupts, you also have to include <avr/interrupts.h>, globally enable the interrupts
     * using `sei();` and implement an Interrupt-Service-Routine using the `ISR()`-macro. Interrupt-vectors for
     * the `ISR()`-macro for Timer/Counter 1 are: `TIMER1_CAPT_vect`, `TIMER1_COMPA_vect`, `TIMER1_COMPB_vect` and
     * `TIMER1_OVF_vect`. The ATmega2560 also has `TIMER1_COMPC_vect`. For the other timer/counters just adjust the
     * number in the `_vect`-macro.
     *
     * \arg \c interruptEnableFlags A bitwise 'or' of the enum-constants that begin with `T16_INT_`. To
     *      enable for example the overflow- and the compare-match-B-interrupts pass
     *      `T16_INT_OVERFLOW | T16_INT_COMP_MATCH_B` as argument.
     */
    void enableInterrupts( Timer16_Interrupts interruptEnableFlags  )
    { *m_timsk |= ((uint8_t)interruptEnableFlags); }

    /*!
     * Disable one or more of the Interrupts of this 16-Bit-Timer-Counter
     *
     * \arg \c interruptEnableFlags A bitwise 'or' of the enum-constants that begin with `T16_INT_`. To disable
     *      for example the overflow- and the compare-match-B-interrupts pass
     *      `T16_INT_OVERFLOW | T16_INT_COMP_MATCH_B` as argument.
     *
     * \see `enableInterrupts`
     */
    void disableInterrupts( Timer16_Interrupts interruptEnableFlags )
    { *m_timsk &= ~((uint8_t)interruptEnableFlags); }

    /*!
     * Clears a pending interrupt-Event of the timer (this is, an eventually set Interrupt-Flag is cleared). Sometimes
     * this is useful before enabling the interrupts.
     *
     * \arg \c interruptEnableFlags A bitwise or of the enum-constants that begin with `T16_INT_`. To clear
     *      for example the pending overflow- and the compare-match-B-interrupt-events pass
     *      `T16_INT_OVERFLOW | T16_INT_COMP_MATCH_B` as argument.
     */
    void clearPendingInterruptEvents( Timer16_Interrupts interruptEnableFlags )
    { *m_tifr |= ((uint8_t)interruptEnableFlags); }


private:

    sfr8Ptr m_tccrna;
    sfr8Ptr m_tccrnb;
    sfr8Ptr m_tccrnc; //TCCRnC-Register not used by this API
    sfr16Ptr m_tcntn;
    sfr16Ptr m_ocrna;
    sfr16Ptr m_ocrnb;
    sfr16Ptr m_ocrnc; //initialized with NULL on ATmega328p
    sfr16Ptr m_icrn;
    sfr8Ptr m_timsk;
    sfr8Ptr m_tifr;
};


/*!
 * Use this macro to initialize a `TimerCounter16Bit`-Object.
 *
 * \arg \c no is the Number of the 16-Bit-Timer/COutner. For the Atmega2560 this is 1, 3, 4, 5, for the
 * Atmega328p `no` must be 1.
 *
 * \see enum `TimerCounter16` and the constructor of `TimerCounter16Bit`
 */
#ifdef OCR1C
#define makeTimerCounter16BitObject( no )    TimerCounter16Bit( &TCCR##no##A, &TCCR##no##B, &TCCR##no##C, &TCNT##no, \
                                                                &OCR##no##A, &OCR##no##B, &OCR##no##C, &ICR##no,     \
                                                                &TIMSK##no, &TIFR##no )
#else
#define makeTimerCounter16BitObject( no )    TimerCounter16Bit( &TCCR##no##A, &TCCR##no##B, &TCCR##no##C, &TCNT##no, \
                                                                &OCR##no##A, &OCR##no##B, NULL, &ICR##no,     \
                                                                &TIMSK##no, &TIFR##no )
#endif


#endif /* TIMER_16_BIT_H_ */
