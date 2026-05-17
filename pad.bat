@echo off
REM pad.bat - Rellena chess.bin a 14336 bytes para XMODEM
setlocal
set file=output\chess.bin
if not exist %file% (
    echo %file% no encontrado. Ejecuta make primero.
    exit /b 1
)
for %%I in (%file%) do set sz=%%~zI
set /a falta=14336 - %sz%
if %falta% gtr 0 (
    echo Rellenando %sz% -^> 14336 bytes (%falta% bytes de ceros)
    powershell -Command "$b=[System.IO.File]::ReadAllBytes('%file%');$pad=New-Object byte[](14336-$b.Length);[System.IO.File]::WriteAllBytes('%file%',($b+$pad))"
) else (
    echo El archivo ya tiene 14336 bytes o mas.
)
