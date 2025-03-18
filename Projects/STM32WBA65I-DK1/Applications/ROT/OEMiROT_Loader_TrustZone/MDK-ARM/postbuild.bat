@ECHO OFF

:: Enable delayed expansion
::setlocal EnableDelayedExpansion

:: arg1 is the build directory
set "projectdir=%~dp0"
pushd %projectdir%\..\..\..\..\ROT_Provisioning
set provisioningdir=%cd%
popd
call "%provisioningdir%\env.bat"
set "operation=%~1"
set "userAppBinary=%projectdir%\..\Binary"
set "cube_fw_path=%projectdir%\..\..\..\..\..\.."
::dummy nsc is generated only in config without secure
set "secure_nsc=%projectdir%\..\Secure_nsclib\secure_nsclib.o"
set maxbytesize=440
set "loader_s=%projectdir%\Secure\STM32WBA65I-DK1_OEMiROT_Loader_TrustZone_S\Exe\Project.bin"
set "loader_ns=%projectdir%\NonSecure\STM32WBA65I-DK1_OEMiROT_Loader_TrustZone_NS\Exe\Project.bin"
set "binarydir=%projectdir%\..\Binary"
set "loader=%binarydir%\loader.bin"
set loader_ns_size=0x6000
set loader_s_size=0x4000
::line for window executable
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %applicfg% (
echo Postbuild with windows executable
goto postbuild
)
:py
::line for python
echo Postbuild with python script
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python=python "
:postbuild
IF "%operation%" == "secure" (
goto secure_operation
)
::Make sure we have a Binary sub-folder in UserApp folder
if not exist "%binarydir%" (
mkdir "%binarydir%"
)

::get size of secure_nsclib.o to determine MCUBOOT_PRIMARY_ONLY flag
FOR /F "usebackq" %%A IN ('%secure_nsc%') DO set filesize=%%~zA

if %filesize% LSS %maxbytesize% (
echo "loader without secure part (MCUBOOT_PRIMARY_ONLY not defined)" >> %projectdir%\postbuild.log
set "command=%python%%imgtool% ass  -i %loader_ns_size% %loader_ns% %loader% >> %projectdir%\postbuild.log 2>>&1"
goto execute_command
)
:: loader with secure and non secure
echo "loader with secure part (MCUBOOT_PRIMARY_ONLY defined)" >> %projectdir%\postbuild.log
set "command=%python%%imgtool% ass -f %loader_s% -o %loader_s_size% -i %loader_ns_size% %loader_ns% %loader% >> %projectdir%\postbuild.log 2>&1"

:execute_command
%command%
IF %errorlevel% NEQ 0 goto :error

if not exist "%loader%" (
exit 1
)
exit 0
:secure_operation
set "nsclib_destination=%projectdir%\..\Secure_nsclib\secure_nsclib.o"
set "nsclib_source=%projectdir%\Secure\STM32WBA65I-DK1_OEMiROT_Loader_TrustZone_S\Exe\Project_import_lib.o"
:: recopy non secure callable .o
set "command=copy %nsclib_source% %nsclib_destination%"
%command%
IF %errorlevel% NEQ 0 goto :error
exit 0
:error
echo "%command% : failed" >> %projectdir%\\postbuild.log
exit 1


