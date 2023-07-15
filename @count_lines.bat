:: �S�\�[�X�̍s���𐔂���
@echo off
@setlocal enabledelayedexpansion

cd /d "%~dp0"
echo �y�\�[�X�R�[�h�s���z

pushd "%~dp0src"

set total=0
for /f "usebackq" %%i in (`dir /b /ad %dir%`) do (
pushd %%i
	@echo | set /p="%%i = "
	call :line_count
	set /a total+=total_lines
popd
)

popd

echo ------------------------------------------------------------
echo total %total% lines
echo.

pause

goto end

::---------------------------------------------------------------
:: �s�����J�E���g
::---------------------------------------------------------------
:line_count
:: �s��
set total_lines=0

for /r %%i in (*.c *.h *.cpp *.hpp *.cxx *.hxx *.inl) do (
	set lines=
	for /f %%a in ('type %%i ^| find /c /v ""') do set /a lines=%%a
	set /a total_lines+=lines
)
echo %total_lines% lines

:end











