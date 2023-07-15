@echo off

::----------------------------------------------------
:: �v���W�F�N�g�t�@�C���𐶐�
::----------------------------------------------------
pushd "%~dp0"
call "@make.bat"
call "@code_format.bat" User
popd

::----------------------------------------------------
:: �\�����[�V�����t�@�C��������
:: �����Ō����邱�ƂŃt�@�C�������ω����Ă��Ή��\
::----------------------------------------------------
cd "%~dp0\.build"
setlocal enabledelayedexpansion

for /f "usebackq" %%i in (`dir /b *.sln`) do set SOLUTION_FILE=%%i
if not exist "%SOLUTION_FILE%" (
	echo �v���W�F�N�g�t�@�C����������܂���Bmake.bat�����s���Ă��������B
	goto error
)

:: �ŐV��VisualStudio����N�����邽�߂ɋN������o�[�W������VisualStudio2019������
set PATH=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio\Installer
for /f "usebackq delims=" %%i in (`vswhere.exe -latest -property productPath`) do (
	set DEVENV_EXE=%%i
)

:: �\�����[�V�������N��
start /min "" "%DEVENV_EXE%" "%SOLUTION_FILE%"

exit

:error
pause
