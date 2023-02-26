@echo off
echo Starting compilation
if not exist .\release\ mkdir release
if exist .\release\*.bin del .\release\*.bin
call F:\Tools\WinAVR\bin\avr-gcc -c -o .\release\RaspberryPower.o -g -O2 -Wall -std=gnu99 -mmcu=atmega88p -DF_CPU=8000000UL RaspberryPower.c
call F:\Tools\WinAVR\bin\avr-gcc -c -o .\release\serial.o -g -O2 -Wall -std=gnu99 -mmcu=atmega88p -DF_CPU=8000000UL serial.c  
call F:\Tools\WinAVR\bin\avr-gcc -g -Wall -O2 -Wl,-Map,.\release\RaspberryPower.map -mmcu=atmega88 -o .\release\RaspberryPower.elf .\release\RaspberryPower.o .\release\serial.o F:\Tools\WinAVR\avr\lib\libm.a
call F:\Tools\WinAVR\bin\avr-objdump -h -S .\release\RaspberryPower.elf > .\release\RaspberryPower.lst
call F:\Tools\WinAVR\bin\avr-objcopy -j .text -j .data -O binary .\release\RaspberryPower.elf .\release\RaspberryPower.bin
@if exist .\release\*.$$$ del .\release\*.$$$
@if exist .\release\*.o del .\release\*.o
@echo on