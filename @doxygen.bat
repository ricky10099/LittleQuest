:: Doxygen��������ѕ\��
@echo off

cd document
"%ProgramFiles%\doxygen\bin\doxygen" .\Doxyfile
start .\html\index.html

exit /b 0
