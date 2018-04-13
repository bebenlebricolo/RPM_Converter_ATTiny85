"D:\Installed Programs\Arduino\hardware\tools\avr\bin\avrdude" -C"D:\Installed Programs\Arduino\hardware\tools\avr\etc\avrdude.conf"  -cstk500v1 -pattiny85 -PCOM11 -b 19200 -U flash:w:"D:\Dossier Perso\Programmation\AtmelStudio\RPM_converter_v0.2\RPM_converter_v0.2\Debug\RPM_converter_v0.2.hex":i -U lfuse:w:0xa2:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m 

PAUSE