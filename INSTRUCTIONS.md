Instructions
============

This modified version of Texas Instruments' BSL-Scripter software for WIN32 allows a target MSP340 microcontroller to be flashed using a generic USB-to-Serial adapter, such as the FT232, CP2102 or CH340, as the hardware interface.  If the "INVOKE" option is included on the "MODE" line, the modified Scripter will directly generate the hardware invoke pattern on the adapter's DTR and RTS lines, which should be connected to the target's /Reset and TEST pins, respectively.

The INVOKE option should be used ONLY for UART flashing of MSP430F5xx, F6xx and FRxx parts, and only with a generic adapter.  It is not needed, and should not be used, if a Rocket or MSP-FET is the interface device.

If INVOKE is used, it is highly likely that the PARITY option is also needed on the MODE line.  Most target parts require 8E1, which the PARITY option causes Scripter to use.

The adapter must provide DTR, RTS, TXD, RXD, Ground, and 3.3V (if needed to power the target). Most generic adapter modules do not bring out both DTR and RTS to their standard 6-pin header.  However, FT232 and CP2102 modules are widely available which provide a header through-hole for RTS along the side of the module. A CH340 module like that has not been found, but the CH340 I/C may be large enough that a wire could be connected from its RTS pin to the CTS header after cutting the existing trace to the CTS header.

Neither these modifications to BSL-Scripter, nor the use of generic USB adapters for flashing, have been evaluated or approved by Texas Instruments.  All relevant source code and executables are Copyright various dates by Texas Instruments, Incorporated, and any further use is subject to the terms, conditions, qualifications and reservations set forth in their original release by Texas Instruments. The original source code and executables for BSL-Scripter, and its User's Guide, are available directly from Texas Instruments:

[http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPBSL_Scripter/latest/index_FDS.html](http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPBSL_Scripter/latest/index_FDS.html)

[http://www.ti.com/lit/slau655](http://www.ti.com/lit/slau655)
