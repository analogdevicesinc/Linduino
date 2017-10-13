
echo off
setlocal
echo Do not use a USB hub. Plug cable directly into PC USB port. Starting...
LinduinoEEPROMProg.exe
echo FTDI EEPROM programmed.
LinduinoCyclePort.exe
echo If any errors appear, press CTRL-C and put Linduino in failed bin.
echo Wait for port to cycle.
pause
GetLinduinoComPort.exe > %TEMP%\comport.txt
set /p ComPort= < %TEMP%\comport.txt
avrdude\avrdude.exe avrdude\avrdude.conf -v -patmega328p -carduino -P\\.\%ComPort% -b115200 -D -Uflash:w:".\files\Tester.cpp.hex":i
echo Start test box.  If LED's indicate failure, put Linduino in failed bin.
pause
avrdude\avrdude.exe avrdude\avrdude.conf -v -patmega328p -carduino -P\\.\%ComPort% -b115200 -D -Uflash:w:".\files\DC590B.cpp.hex":i
del %TEMP%\comport.txt
echo DC590B.cpp.hex loaded into Linduino.
endlocal