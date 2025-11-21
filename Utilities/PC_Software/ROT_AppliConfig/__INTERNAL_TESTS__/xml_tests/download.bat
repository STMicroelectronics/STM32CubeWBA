:: Getting the STM32CubeProgammer CLI path
@echo off
set projectdir=%~dp0
call "%projectdir%env.bat"
call "%provisioning_dir%\env.bat"

:: Environment variable for log file
set current_log_file="%projectdir%download.log"
echo. > %current_log_file%

:: Environment variables for binary
set path_binary="%projectdir%\Binary\appli.bin"

:: Environment variables for STM32CubeProgammer
set vb=1
set reset=-hardRst
set connect=-c port=SWD speed=fast ap=1 mode=HotPlug -vb %vb%
set write_dbgmcu=-w32dbgmcu

:: Value to jump in bootloader - STBL in ASCII
set stbl_dbgmcu=0x5354424c

:: Reset value
set raz_dbgmcu=0x00000000

::Address to download the raw binary
set ns_code_image=
echo =====
echo ===== Downloading of SMAK_Appli
echo =====
:: ============================================================= Start =============================================================
echo =====     Start programming

echo    * Step 0 : Verification of the binary >> %current_log_file%
if not exist %path_binary% (
echo %path_binary% not exist, please build your application >> %current_log_file%
goto error
)

echo. >> %current_log_file%
echo    * Step 1 : Jump to bootloader >> %current_log_file%
%stm32programmercli% %connect% %write_dbgmcu% %stbl_dbgmcu% %reset% >> %current_log_file%
IF %errorlevel% NEQ 0 goto :error
timeout 1 /NOBREAK >nul


%stm32programmercli% %connect% -d %path_binary% %ns_code_image% -v >> %current_log_file%
IF %errorlevel% NEQ 0 goto :error

echo. >> %current_log_file%
echo    * Step 3 : Reset to swap download area and active area >> %current_log_file%
%stm32programmercli% %connect% %write_dbgmcu% %raz_dbgmcu% %reset% >> %current_log_file%
IF %errorlevel% NEQ 0 goto :error
timeout 1 /NOBREAK >nul

:: ============================================================= End =============================================================
:: All the steps were executed correctly
echo =====
echo =====     The non-secure application is correctly updated
echo =====
if [%1] neq [AUTO] cmd /k
exit 0

:: Error when script is executed
:error
echo =====
echo =====     Error occurred.
echo =====     See %current_log_file% for details. Then try again.
echo =====
if [%1] neq [AUTO] cmd /k
exit 1

