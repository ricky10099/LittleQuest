@echo off
@setlocal enabledelayedexpansion

pushd "%~dp0"

set CLANG_FORMAT_CONFIG=.clang-format-reference

echo �S�\�[�X�R�[�h�𐮌`���܂�

:: ���[�U�[��`�t�@�C���ŕϊ�
if "%1"=="User" (
    echo ���[�U�[��`�̐��`�����s���܂�
    set CLANG_FORMAT_CONFIG=.clang-format
)

::---------------------------------------------------------------
:: VisualStudio����clag-format�̃p�X�����o
::---------------------------------------------------------------
set VSWHERE_PATH="C:/Program Files (x86)/Microsoft Visual Studio/Installer/vswhere.exe"

if not exist %VSWHERE_PATH% (
	echo VisualStudio���C���X�g�[������Ă��܂���.
	goto error_finish
)

:: VisualStudio���C���X�g�[������Ă���p�X���擾
for /f "usebackq delims=" %%i in (`%VSWHERE_PATH% -latest -property installationPath`) do (
	set VS_PATH=%%i
)

:: VisualStudio����LLVM�̃p�X
set VS_LLVM_PATH=%VS_PATH%\VC\Tools\Llvm\x64\bin

:: clang-format�̃p�X
set CLANG_FORMAT_PATH="%VS_LLVM_PATH%\clang-format.exe"

::---------------------------------------------------------------
:: clang-format�����s�ł��邩�ǂ����`�F�b�N
::---------------------------------------------------------------
if not exist %CLANG_FORMAT_PATH% (
	echo clang-format��������܂���.
	goto error_finish
)

:: ���[�J����clang-format�̃t���p�X�𗘗p (style�Ƀt�@�C���w��ł���@�\��LLVM14����݂̂̂���)
:: VisutalStudio��LLVM14�x�[�X�ɂȂ����炱�̏����͕s�v�ɂȂ�܂�
::for /f "delims=" %%f in ('dir /b /s  bin\clang-format.exe') do (
::  set CLANG_FORMAT_PATH="%%f"
::)

:: �o�[�W�����\��
%CLANG_FORMAT_PATH% --version

::---------------------------------------------------------------
:: src�����̂��ׂẴ\�[�X�R�[�h��clang-format�����s
::---------------------------------------------------------------
pushd "%~dp0src"

for /f "delims=" %%f in ('dir /b /s src *.c *.cpp *.cxx *.cc *.hpp *.h *.hxx *.inl') do (
  set /p X="." < nul
  start /B "" %CLANG_FORMAT_PATH% -i -style=file:%CLANG_FORMAT_CONFIG% "%%f"
)

echo �������܂���.

goto finish

:error_finish
popd
pause

exit /b 1

:finish
popd

exit /b 0
