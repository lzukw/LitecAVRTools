# USART-module #

The USARTs of the ATmega-Microcontroller are normally used as serial interface
(RS232). The ATmega328p has one USART (USART0), the ATmega2560 has four USARTs
(USART0, USART1, USART2 and USART3).

To use the USART-module of the LitecAVRTools-Library, add the files
"GpioPinMacros.h" "Usart.h" and "Usart.cpp" to your project, and 
`#include "Usart.h"`. 

There is only an object-oriented interface. 

## Initializing the USART ##

To initalize USART0 to 9600 Baud, 
8-Bit-Dataframes, no parity-Bit and one stop-Bit (8N1), first create an
object, using the `makeUsartObject`-macro and then call the `init`-method on 
the object:
```C
Usart usart0 = makeUsartObject( 0 );
usart0.init( 9600 );
```

To use other Data-Frames (for examle 115200 Baud, 7 Bits, Even Parity and 
2 Stop-Bits) pass a second argument to the `init`-method:
```C
usart0.init( 115200, cUsart_7E2 );
```

## High-level methods ##

The methods `usartPrintf` and `usartScanf` behave like the ordinary `printf` and
`scanf`-functions:

```C
int number;
usart0.usartPrintf("Enter an integer number: ");
usart0.usartScanf("%d", &number);
usart0.usartPrintf("%d*2=%d\r\n", number, number*2);
```

## Low-level methods ##

There are blocking and non-blocking methods to transmit and receive a byte.
As argument for the transmit-methods a `char` or a `uint8_t` can be used:

```C
char usartChar = 'A';
usart0.transmitByte( usartChar );
uint8_t usartByte = 65;
usart0.transmitByte( usartByte );

int8_t returnValue = usart0.transmitByteNonBlocking( usartByte );
```

The non-blocking method returns 0, if uint8_t/char to transmit has successfully 
been queued for transmission. Otherwise the `transmitByteNonBlocking`-method
returns -1, and does not transmit the uint8_t/char.

There is a blocking and a non-blocking method for receiving a byte:
```C
int16_t received = usart0.receiveByteNonBlocking();
uint8_t receivedByte = usart0.receiveByte();
```
The method `receiveByte()` blocks unitl a byte has been received on USART0. 
The return-value `received` of the non-blocking method 
`receiveByteNonBlocking()` is -1, if no byte has been received on USART0 yet.
Otherwise, if there has been received a byte, it is cast to an int16_t and 
returned. 

To test, if a byte has been recieved, the method `byteAvailable()` can be used.
This method returns non-zero, if a byte is in the internal receive-buffer and
has not been fetched yet by `receiveByte()` or `receiveByteNonBlocking()`.

Each USART has three Interrupt-Sources (Receive Complete, Transmit Complete,
Usart-Data-Register empty). To enable or disable them, use the
method `enableOrDisableInterrupts`. This method has three arguments. Pass a
1 or a 0 for each of these arguments to enable/disable the corresponding 
Interrupt (1 = enable, 0 = disable).

The method `getReceiveErrors()` returns an uint8_t-value. If no erros have 
occured, the this value is 0. Otherwise it is an or-ed value of the three
possible errors: `cUsart_ParityError`, `cUsart_DataOverrunError` and 
`cUsart_FrameError`.

