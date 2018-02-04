# LitecAVRTools - GPIO-module #

There are four ways to program the GPIO-Pins. 
- Either C-"functions" or C++-objects can be used. 
- Either single GPIO-Pins or whole GPIO-Ports (8 pins) can be programmed. 

It is possible to mix the four different ways.

## Programming single GPIO-Pins using C-"functions" ##

Set the mode of the GPIO-Pin PA2 to output. Then put out a High-voltage-
level on PA2.
```C
setGpioPinModeOutput( GpioPin( A, 2) );
writeGpioPinDigital( GpioPin( A, 2), cHigh );
```
Instead of using the constants `cHigh` and `cLow`, the values 1 and 0 can be
used.

Intstead of using `writeGpioPinDigital()` the voltage-levels of the output PA2
can also be changed using:
- `setGpioPinHigh( GpioPin( A, 2 ) );`
- `setGpioPinLow( GpioPin( A, 2 ) );`
- `toggleGpioPin( GpioPin( A, 2) );`

Set the mode of GPIO-Pin PD2 to input (without internal pull-up-resistor):
```C
setGpioPinModeInput( GpioPin( D, 2) );
```

Set the mode of GPIO-Pin PD3 to input with activated internal pull-up-resistor:
```C
setGpioPinModeInputPullup( GpioPin( D, 3) );
```

Read in the voltage-level of pin PD3:
```C
if (readGpioPinDigital( GpioPin( D, 3) ) == cLow)
{ ... }
```

Check the mode (input or output) of a GPIO-pin:
`isGpioPinModeOutput( GpioPin ( A, 2) )` returns non-zero (true) if PA2 is an
output, and returns 0 (false) otherwise.
`isGpioPinModeInput( GpioPin ( D, 3) )` returns non-zero (true) if PD3 is an
input, and returns 0 (false) otherwise.

For the number in the macro `GpioPin( <Letter>, <Number> )` a variable
can be used. For example the following is possible:
```C
int i;
for (i=0; i<4; i++)
{
    writeGpioPinDigital( GpioPin( D, i), cLow );
}
```

It should be noted, that the above explained functions are not functions,
but macros. Therefore avrgcc normally does not show syntax-errors where they
occur, but in the macro-definition in GpioPinMacros.h

## Programming single GPIO-Pins using C++-objects ##

First an Object must be created using the macro `makeGpioPinObject`. Create
a GpioPinObject for GPIO-Pin PA2, set its mode to output, then put out a 
high-voltage-level:
```C
GpioPinObject ledPin = makeGpioPinObject( GpioPin( A, 2) );
ledPin.setModeOutput();
ledPin.writeDigital( cHigh );
```
The following line toggles the voltage-level of `ledPin` output-pin PA2:
```C
ledPin.toggle();
```
Create an Object for GPIO-Pin PD2. Set its mode to input without internal 
pull-up-resistor, and read in its voltage-level.
```C
GpioPinObject buttonPin = makeGpioPinObject( GpioPin( D, 2) );
buttonPin.setModeInput();
uint8_t level = buttonPin.readDigital();
```
The result `level` will either be 0 (cLow) or 1 (cHigh).

To set a GpioPinObject's mode to input with activated internal pullup-
resistor, use the method `setModeInputPullup()` instead of `setModeInput()`.

Also there are the methods `isOutput()` and `isInput()`, which return non-zero
(true) or 0 (false) if the pin's mode is actually set to output / input.



## Programming whole GPIO-Ports using C-"functions" ##

The following functions (macros) all receive a `mask`-value as last argument.
Only those GPIO-Pins of a port are affected, whose corresponging bit in
`mask` is one. The other pins remain unchanged.

Set mode of pins on port A: Set the mode of the Pins PA5, PA4 and PA3 to output, 
the mode of PA2, PA1, PA0 to input, and leave the mode of the reamining pins
(PA7 and PA6) unchanged:
```C
setGpioPortMode( GpioPort( A ), 0x38, 0x3F );
```
Explanation: 

| Pin:       |    PA7    |    PA6    |   PA5  |   PA4  |   PA3  |  PA2  |  PA1  |  PA0  |
|------------|:---------:|:---------:|:------:|:------:|:------:|:-----:|:-----:|:-----:|
| mode=0x38: |     0     |     0     |    1   |    1   |    1   |   0   |   0   |   0   |
| mask=0x3F: |     0     |     0     |    1   |    1   |    1   |   1   |   1   |   1   |
| Result:    | unchanged | unchanged | output | output | output | input | input | input |

Turn on internal pullup-resistor on input-pin PA1 and turn it off on input-pin 
PA0, leave the pullup of input PA2 unchanged (second argument 
`mask`=0x03=0b00000011, first argument `pullup`=0x02=0b00000010):
```C
setGpioPortPullup( GpioPort( A ), 0x02, 0x03 );
```

Write a High-level to output-Pins PA5 and PA4, and a low-level to output-pin
PA3 (second argument `mask`=0x38=0b00111000, first argument 
`voltageLevels`=0x30=0b00110000:
```C
writeGpioPort( GpioPort( A ), 0x30, 0x38 );
```

Read in voltage-levels of pins PA2 and PA0 (`mask`=0x05=0b00000101):
```C
uint8_t levels = readGpioPort( GpioPort( A ), 0x05 );
```
The return-value `levels` is `0b00000a0b` where bits `a` and `b` are 1 if the 
voltage-level on the corresponding pin is high, and 0 if the voltage-level is 
low. The voltage-level of the other pins ignored, since the corresponding bits
in the `mask`-argument are 0.

Toggle the voltage-levels of the output-Pins PA4 and PA3 
(`mask`=0x18=0b00011000):
```C
toggleGpioPort( GpioPort( A ), 0x18 );
```

## Programming whole GPIO-Ports using C++-objects ##

The class `GpioPort` offers the same functionality as the "functions" (macros) 
described in the last section.

The `mask`-argument to the `GpioPort`-methods has a default-value of 0xFF. To
modify all 8 pins of the port, you can ommit passing a value for `mask`.

Set mode of pins on port A: Set the mode of the Pins PA5, PA4 and PA3 to output, 
the mode of PA2, PA1, PA0 to input, and leave the mode of the reamining pins
(PA7 and PA6) unchanged.
```C
GpioPortObject portA = makeGpioPortObject( GpioPort( A ) );
portA.setMode( 0x38, 0x3F );
```

Turn on internal pullup-resistor on input-pin PA1 and turn it off on input-pin 
PA0, leave the pullup of input PA2 unchanged 
```C
portA.setPullup(  0x02, 0x03 );
```

Write a High-level to output-Pins PA5 and PA4, and a low-level to output-pin
PA3:
```C
portA.writeDigital( 0x30, 0x38 );
```

Read in voltage-levels of pins PA2 and PA0
```C
uint8_t levels = portA.readDigital( GpioPort( A ), 0x05 );
```

Toggle the voltage-levels of the output-Pins PA4 and PA3 :
```C
portA.toggle( 0x18 );
```

Furthermore, there are two methods `getMode()` and `getPullup()`. 





