@echo Starting compilation
SET COMPPATH=C:\Develop\WinAVR
@if exist %1.bin del %1.bin
@if exist %1.elf del %1.elf
@%COMPPATH%\bin\avr-gcc -c -o %1.o -g -O2 -Wall -mmcu=atmega88 %1.c 
@%COMPPATH%\bin\avr-gcc -g -Wall -O2 -Wl,-Map,%1.map -mmcu=atmega88 -o %1.elf %1.o  %COMPPATH%\avr\lib\libm.a
@%COMPPATH%\bin\avr-objdump -h -S %1.elf > %1.lst
@%COMPPATH%\bin\avr-objcopy -j .text -j .data -O binary %1.elf %1.bin
@if exist %1.$$$ del %1.$$$
@if exist %1.o del %1.o
@echo on