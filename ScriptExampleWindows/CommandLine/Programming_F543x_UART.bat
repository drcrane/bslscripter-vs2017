REM Assume the device is empty, so the -b will result in "password is correct"
-n f543x_family -i [COM188,UART,9600] -b -e ERASE_ALL -w blinkLED_f5438.txt -v -z [SET_PC,0x5c00]