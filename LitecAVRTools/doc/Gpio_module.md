# LitecAVRTools - GPIO-module #

There are four ways to program the GPIO-Pins. Either C-"functions" or 
C++-objects can be used. Either single GPIO-Pins or whole GPIO-Ports (8 pins)
can be programmed. It is possible to mix these four different ways.

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

It should be noted, that the above explained functions are not functions,
but macros. Therefore avrgcc normally does not show syntax-errors where they
occur, but in the macro-definition in GpioPinMacros.h

## Programming single GPIO-Pins using C++-objects ##



## Programming whole GPIO-Ports using C-"functions" ##

The following functions (macros) all receive a `mask`-value as last argument.
Only those GPIO-Pins of a port are affected, whose corresponging bit in
`mask` is one. The other pins remain unchanged.

Set the mode of some (or all) of the 8 GPIO-Pins of a Port:



## Programming whole GPIO-Ports using C++-objects ##


