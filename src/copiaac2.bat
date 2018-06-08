@echo off
if not exist %2 goto primeracopia
diffile %1 %2
if ERRORLEVEL 255 goto copia
goto fin
:primeracopia
echo El fichero no existe, se copia por primera vez
:copia
copy %1 c:
:fin

