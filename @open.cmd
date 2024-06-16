@echo off
@setlocal enabledelayedexpansion
chcp 65001 > nul

::----------------------------------------------------
:: プロジェクトファイルを生成
::----------------------------------------------------
pushd "%~dp0"
call "@make.cmd"
call "@code_format.cmd" User
popd

::----------------------------------------------------
:: ソリューションファイルを検索
:: 検索で見つけることでファイル名が変化しても対応可能
::----------------------------------------------------
cd "%~dp0\.build"
setlocal enabledelayedexpansion

for /f "usebackq" %%i in (`dir /b *.sln`) do set SOLUTION_FILE=%%i
if not exist "%SOLUTION_FILE%" (
	echo プロジェクトファイルが見つかりません。@make.cmdを実行してください。
	goto error
)

:: 最新のVisualStudioから起動するために起動するバージョンでVisualStudio2019を検索
set PATH=%PATH%;C:\Program Files (x86)\Microsoft Visual Studio\Installer
for /f "usebackq delims=" %%i in (`vswhere.exe -latest -property productPath`) do (
	set DEVENV_EXE=%%i
)

:: ソリューションを起動
start /min "" "%DEVENV_EXE%" "%SOLUTION_FILE%"

exit

:error
pause
