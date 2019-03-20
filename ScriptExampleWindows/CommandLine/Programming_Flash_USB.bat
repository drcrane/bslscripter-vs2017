REM the device is empty, so the -b will result in "password is correct"
REM first step is to load the RAM_USB_BSL.txt and then jump to the application
REM of RAM_USB_BSL
-n 5xx -i [USB] -b -e ERASE_ALL -w RAM_USB_BSL.txt -v [SET_PC,0x2504]
REM the initialization and download the application
-n 5xx -i [USB] -b -w blink_f5529.txt -v [SET_PC,0x4400]