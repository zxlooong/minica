::@echo off 
set BatPath=%~dp0 
cd %BatPath%
%~d0

copy /B /Y .\Debug\MiniCa.exe              ..\..\Mc2Demo\mc
