@echo Starting compilation
@if exist %1.bin del %1.bin
@if exist %1.elf del %1.elf
@F:\Tools\WinAVR\bin\avr-gcc -c -o %1.o -g -O2 -Wall -mmcu=atmega88 %1.c 
@F:\Tools\WinAVR\bin\avr-gcc -g -Wall -O2 -Wl,-Map,%1.map -mmcu=atmega88 -o %1.elf %1.o  F:\Tools\WinAVR\avr\lib\libm.a
@F:\Tools\WinAVR\bin\avr-objdump -h -S %1.elf > %1.lst
@F:\Tools\WinAVR\bin\avr-objcopy -j .text -j .data -O binary %1.elf %1.bin
@if exist %1.$$$ del %1.$$$
@if exist %1.o del %1.o
@echo on