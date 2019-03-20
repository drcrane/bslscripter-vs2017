REM Assume the device is empty, so the -b will result in "password is correct"
-n FRxx -i [COM188,I2C,400000] -b -w blinkLED_FR6989.txt -v -z [SET_PC,0x4400]