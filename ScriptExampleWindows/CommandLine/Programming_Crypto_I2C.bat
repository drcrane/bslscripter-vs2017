REM Assume the device is empty, so the -b will result in "password is correct"
-n Crypto -i [COM188,I2C,400000] -b -w blinkLED_Crypto.txt -z [RESET]