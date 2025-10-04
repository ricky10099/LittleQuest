::============================================================================
:: Doxygen生成および表示
::============================================================================
@echo off
@setlocal enabledelayedexpansion
chcp 65001 > nul

cd document
"E:\Program Files\doxygen\bin\doxygen" .\Doxyfile
start .\html\index.html

exit /b 0
