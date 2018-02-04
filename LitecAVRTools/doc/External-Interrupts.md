# External Interrupts #

There are some dedicated GPIO-Pins, that have external-interrupt-capability (for 
example on the Atmega2560 the pin PD2 has the alternative function INT2). 
Voltage-Level changes on the INTx-Pins can cause external Interrupts.

The following things have to be done, to use external Interrupts with 
LitecAVRTools:

- Add the files ExternalInterrupts.h and ExternalInterrupts.cpp to your project
- #include <avr/interrupt.h> and #include "ExternalInterrupts.h"
- Make the INTx-Pin you want to use an input (also decide, if you have to 
  activate the internal pullup-resistor).
- Decide, which voltage-level-changes cause an interrupt: a rising edge 
  (voltage-change from low to high), a falling edge, both edges or a low-level. 
  For example, to set the sensetivity of the INT3-interrupt to "rising edge", 
  use:
  `setExtIntEventType( 3, EXTINT_RISING_EDGE);`
- Enable the INTx-Interrupt. For example, to enable the INT3-Interrupt use:
  `enableExtInt( 3 );`
  You can later disable the Interrupt using `disableExtInt( 3 );`
- Globally enable interrupts using `sei();` (this is from <avr/interrupt.h>)
  You can later disable all interrupts using `cli()`.
- Write program-code for the Interrupt-service-routine using the `ISR()`-macro 
  from <avr/interrupt.h>. The vectors for the external interrupts are called 
  `INTn_vect`, where n is the number of the corresponding external interrupt.

Data exchange between an interrupt-service-routine and the main-program is done
via global variables. If a global variable is written in the 
interrupt-service-routine, you must declare this variable as `volatile`.
  
## Simple Example ##
Falling edges on Pin INT3 (which is GPIO-Pin PD3 on the ATmega2560) shall cause
an Interrupt. In the interrupt-service-routine a global variable `counter` is 
incremented. The main-Program puts out the value of `counter` every second via 
`usartPrintf()` on the serial interface.

```C
#include <avr/interrupt.h>
#include <util/delay.h>

#include "GpioPinMacros.h"
#include "Usart.h"
#include "ExternalInterrupts.h"

volatile int counter=0;

int main(void)
{
   //make the GPIO-Pin PD3 (INT3) an input. Since we connect a button, we
   //activate the internal pullup-resistor.
   setGpioPinModeInputPullup( GpioPin( D, 3) );

   //Initialize the RS232-Interface
   Usart usart0 = makeUsartObject( 0 );
   usart0.init(9600);
   
   //initialize the external Interrupt INT3: set type, and enable it.
   setExtIntEventType( 3, EXTINT_FALLING_EDGE );
   enableExtInt( 3 );

   //globally enable Interrupts.
   sei();

    while (1) 
    {
         usart0.usartPrintf( "counter=%d\r\n", counter );
         _delay_ms(1000);
    }
}

//The interrupt-service-routine with the corresponding vector
ISR(INT3_vect)
{
   counter++;
}
```

## Object-oriented interface ##

Instead of using the functions `setExtIntEventType()`, `enableExtInt()`, 
`disableExtInt()` and `clearPendingExtIntEvent()`, an instance of the 
`ExtInt`-class can be used. The following example uses external Interrupt
number 3 (INT3).

```C
ExtInt extInt3 = ExtInt( 3, EXTINT_FALLING_EDGE );
extInt3.enable();
```

The event-type is set, when creating an object. But later it can be changed,
using the `setExtIntEventType`-method. To clear a pending Interrupt-Event
before enabling the external Interrupt, the method `clearPendingEvent()` can
be used.