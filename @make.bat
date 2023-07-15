@echo off
@setlocal enabledelayedexpansion

pushd "%~dp0"
call "@cleanup.bat" warm
popd

::---------------------------------------------------------------
:: Git�R�~�b�g�t�b�N��K�p
::---------------------------------------------------------------
pushd "%~dp0"

if exist .git (
    copy bin\pre-commit .git\hooks > nul
)

popd

::---------------------------------------------------------------
:: clang-format��`�t�@�C�����R�s�[
::---------------------------------------------------------------
pushd "%~dp0\src"

if not exist .clang-format (
    copy .clang-format-reference .clang-format > nul
    echo .clang-format�t�@�C�����쐬���܂���.
)

popd

::---------------------------------------------------------------
:: .build�t�H���_���쐬
:: ���ϐ�BUILD_PATH��ݒ肵���ꍇ�͍쐬����ړ��\�B
::---------------------------------------------------------------
pushd "%~dp0"

:: ���łɃt�H���_�����݂��Ă�����쐬���X�L�b�v
if exist .build goto skip

:: �J�X�^���r���h�p�X�̊��ϐ������݂��Ȃ������ꍇ�̓X�L�b�v
if not defined BUILD_PATH goto skip

echo ���ϐ�BUILD_PATH���ݒ肳��Ă��܂��B%BUILD_PATH%
echo BUILD_PATH=%BUILD_PATH%

:: �V���{���b�N�����N�����ɂ͊Ǘ��Ҍ������K�v�Ȃ��ߎ������i
whoami /priv | find "SeDebugPrivilege" > nul
if %errorlevel% neq 0 (
	echo �Ǘ��Ҍ���������܂���ł����B�Ǘ��Ҍ����ɏ��i���Ď��s���܂�
	powershell start-process "`%~0" -verb runas -wait
	goto skip
)

echo �Ǘ��Ҍ����Ɏ����ŏ��i�ł��܂����B

:: �e���|�����t�H���_�p��GUID���擾
for /f %%a in ('powershell -command "$([guid]::NewGuid().ToString())"') do ( set GUID=%%a )

:: �V���{���b�N�����N�ڑ���̃t�H���_����
if not exist "%BUILD_PATH%\.build\%GUID%" (
	md "%BUILD_PATH%\.build\%GUID%"
)

:: �V���{���b�N�����N�쐬
mklink /d .build "%BUILD_PATH%\.build\%GUID%"

:�F�@�I��
exit /b 0

:skip
::---------------------------------------------------------------
:: �v���W�F�N�g�t�@�C���𐶐�
::---------------------------------------------------------------

bin\premake5.exe vs2022

if %ERRORLEVEL% neq 0 (
	echo.
	echo �G���[���������܂����BLua�X�N���v�g���m�F���Ă��������B
	echo.
	echo ==========
	echo "�`�`/premake5.lua: No such file or directory" �ƐԂ������ŕ\�������ꍇ�̓t�H���_�p�X�ɑS�p�������܂܂�Ă��Ȃ���
	echo �z�u�ꏊ���m�F���Ă��������B
	echo.
	echo [���݂̔z�u�ꏊ]
	cd
	echo.
	echo �S�p�����i���{��Ȃǁj���܂܂�Ă��܂��B
	echo.
	echo ==========
	echo.
	pause
)

popd

:finish
exit /b 0
