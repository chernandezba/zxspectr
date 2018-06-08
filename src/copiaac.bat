echo off
echo Se van a copiar los datos modificados de la unidad A al disco duro.
echo Si desea abortar la operaci¢n, pulse CTRL+C.
pause
pause
cd a:\emulador\zxspectr
a:
fsa/s/d *.asm,*.inc,*.mac,*.txt,*.c,*.rom,*.scr,COPYING c:copiaac2 # c:#
c:
