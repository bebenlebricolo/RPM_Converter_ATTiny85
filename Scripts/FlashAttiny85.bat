@echo off
REM ----------------------------TEMPLATE ---------------------------------
REM set AVRDUDE_PATH=your/avrdude/installation/path  e.g. : C:\Programms\Arduino\hardware\tools\avr\bin\avrdude
REM set AVRDUDE_CONF=avrdude.conf configuration file e.g. : C:\Programms\Arduino\hardware\tools\avr\etc\avrdude.conf
REM set PORT=your_port e.g : PCOM11
REM set HEX_PATH=path/to/your/hex/file e.g. : C:\Users\Superman\Is\A\SuperHero\RPM_Converter\ATTiny85_Binaries\RPM_Converter.hex

REM %AVRDUDE_PATH% -C %AVRDUDE_CONF%  -cstk500v1 -pattiny85 -%PORT% -b 19200 -U flash:w:%HEX_PATH%:i -U lfuse:w:0xa2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m 
REM Note for fuses :
REM flash:w:<hexpath>:i -> write firmware to flash memory (erase first)
REM CKOUT : clock output enable (to monitor the hardware clock) on PB4
REM SUT0 : select startup time (6CK/14CK)
REM CKSEL0-2-3 -> Select clock source (8MHz internal crystal oscillator)
REM SPIEN -> SPI enable (not used)

REM Useful link for fuses : http://www.engbedded.com/fusecalc

set AVRDUDE_PATH="D:\Installed Programs\Arduino\hardware\tools\avr\bin\avrdude"
set AVRDUDE_CONF="D:\Installed Programs\Arduino\hardware\tools\avr\etc\avrdude.conf"
set PORT=PCOM10
set HEX_PATH="D:\Dossier Perso\Programmation\AtmelStudio\RPM_Converter\ATTiny85_Binaries\RPM_Converter.hex"

@echo on

%AVRDUDE_PATH% -C %AVRDUDE_CONF%  -cstk500v1 -pattiny85 -%PORT% -b 19200 -U flash:w:%HEX_PATH%:i -U lfuse:w:0xa2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m 

PAUSE