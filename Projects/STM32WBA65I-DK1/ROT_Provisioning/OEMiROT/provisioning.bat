@ECHO OFF
:: Get the escape character (for ANSI colors)
for /f %%a in ('echo prompt $E^| cmd') do set "ESC=%%a"

:: Getting the CubeProgammer_cli path
call ../env.bat
set cube_fw_path=%cube_fw_path:"=%

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set "projectdir=%~dp0"
set oemirot_config_xml="%projectdir%Config\OEMiRoT_Config.xml"

:: CubeProgammer path and input files
set ob_flash_programming="ob_flash_programming.bat"

:: OEM2 key path
set oem2_password="./Keys/oem2_password.txt"

:: Log files
set ob_flash_log="ob_flash_programming.log"
set provisioning_log="provisioning.log"

:: Remove all log files (*.log)
if exist *.log del /f /q *.log

:: Environment variable for path project
set appli_dir=..\..\%oemirot_appli_path_project%
set boot_dir=..\..\%oemirot_boot_path_project%

:: Get config updated by OEMiROT_Boot
set img_config=%projectdir%/img_config.bat

set s_data_xml="%projectdir%Images\OEMiROT_S_Data_Image.xml"
set ns_data_xml="%projectdir%Images\OEMiROT_NS_Data_Image.xml"
set s_data_init_xml="%projectdir%Images\OEMiROT_S_Data_Init_Image.xml"
set ns_data_init_xml="%projectdir%Images\OEMiROT_NS_Data_Init_Image.xml"

set boot_cfg_h="%cube_fw_path%\Projects\STM32WBA65I-DK1\Applications\ROT\OEMiROT_Boot\Inc\boot_hal_cfg.h"
set flash_layout="%cube_fw_path%\Projects\STM32WBA65I-DK1\Applications\ROT\OEMiROT_Boot\Inc\flash_layout.h

:: Initial configuration
set connect_no_reset=-c port=SWD mode=HotPlug
set connect_reset=-c port=SWD mode=UR

:: Check if Python is installed
python3 --version >nul 2>&1
if %errorlevel% neq 0 (
 python --version >nul 2>&1
 if !errorlevel! neq 0 (
    echo Python installation missing. Refer to Utilities\PC_Software\ROT_AppliConfig\README.md
    goto :error
 )
  set "python=python "
) else (
  set "python=python3 "
)

:: Environment variable for AppliCfg
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"


:: Display Boot path
echo =====
echo ===== Provisioning of OEMiRoT boot path
echo ===== Application selected through env.bat:
echo ===== %oemirot_appli_path_project%
echo %ESC%[91m===== Python and some python packages are required to execute this script: Refer to%ESC%[0m
echo %ESC%[91m===== Utilities\PC_Software\ROT_AppliConfig\README.md for more details%ESC%[0m
echo =====
echo.

:: Check if boot path exists
if not exist %boot_dir% (
echo =====
echo ===== Wrong Boot path: %boot_dir%
echo ===== please modify the env.bat to set the right path
goto step_error
)

:: Check if oem2 password path exists
if not exist %oem2_password% (
echo =====
echo ===== Wrong OEM 2 password path: %oem2_password%
echo ===== please put the right path
goto step_error
)

:: ============================================================= RDP selection =============================================================
echo Step 1 : Configuration management
:: RDP selection
:define_rdp_level
set "action=Define RDP level"
echo    * %action%
set /p "RDP_level=      %USERREG% [ 0 | 1 | 2 ]: "

if /i "%RDP_level%" == "0" (
echo.
set rdp_value=0xAA
set rdp_str="OB_RDP_LEVEL_0"
goto no_oem2_key
)

if /i "%RDP_level%" == "1" (
echo.
set rdp_value=0xBB
set rdp_str="OB_RDP_LEVEL_1"
goto no_oem2_key
)

if /i "%RDP_level%" == "2" (
echo.
set rdp_value=0xCC
set rdp_str="OB_RDP_LEVEL_2"
goto provisioning_oem2_key
)

echo        WRONG RDP level selected
echo;
goto define_rdp_level

:: ============================================================ OEM2 key Setting ==========================================================
:provisioning_oem2_key
echo    * OEM2 key setup
echo        Open oem2_password file and put OEM2 key(Default path is \ROT_Provisioning\OEMiRoT\Keys\oem2_password.txt)
echo        Warning: Default OEM2 keys must NOT be used in a product. Make sure to regenerate your own OEM2 keys!
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

:no_oem2_key
:: Step to update RDP Level in boot_hal_cfg
:: Replace RDP level in boot_hal_cfg
set "action=Update RDP Level in boot_hal_cfg"
set "command=%python%%applicfg% modifyfilevalue --variable OEMIROT_OB_RDP_LEVEL_VALUE --value %rdp_str% %boot_cfg_h% --str"
%command%
IF !errorlevel! NEQ 0 goto :step_error

:: Clean OEMuROT configuration
set "action=Update oemurot_enabled in %img_config%"
set "command=%python%%applicfg% definevalue --name=oemurot_enabled  --value=0 --decimal %img_config%"
%command%
IF !errorlevel! NEQ 0 goto :step_error

set "action=Comment OEMUROT_ENABLE in %flash_layout%"
set "command=%python%%applicfg% setdefine -a comment -n OEMUROT_ENABLE -v 1 %flash_layout%"
%command%
IF !errorlevel! NEQ 0 goto :step_error

::Steps to generate OEMiRoT_Keys.bin file
echo    * OEMiROT keys configuration
echo        From TrustedPackageCreator (OBkey tab in Security panel)
echo        Select OEMiRoT_Config.xml(Default path is /ROT_Provisioning/OEMiROT/Config/OEMiRoT_Config.xml)
echo        Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys!
echo        Update the configuration (if/as needed) then generate OEMiRoT_Keys.bin file
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul
%stm32tpccli% -obk %oemirot_config_xml% >> %provisioning_log% 2>>&1
if !errorlevel! neq 0 goto :step_error

:: ========================================================= Images generation steps =======================================================
echo Step 2 : Images generation
echo    * Boot firmware image generation
echo        Open the OEMiROT_Boot project with preferred toolchain and rebuild all files.
echo        At this step the project is configured for OEMiROT boot path.
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

call %img_config%

echo    * Code firmware image generation
IF "%app_full_secure%" == "1" (
echo        Open the OEMiROT_Appli project with preferred toolchain.
echo        Rebuild the Secure project. The %oemirot_appli_secure% and oemirot_tz_s_app_enc_sign.bin files are generated with the postbuild command.
) else (
echo        Open the OEMiROT_Appli_TrustZone project with preferred toolchain.
echo        Rebuild the Secure project. The %oemirot_appli_secure% and oemirot_tz_s_app_enc_sign.bin files are generated with the postbuild command.
echo        Rebuild the NonSecure project. The %oemirot_appli_non_secure% and oemirot_tz_ns_app_enc_sign.bin files are generated with the postbuild command.
)
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

echo    * Data secure generation (if Data secure image is enabled)
echo        Select OEMiRoT_S_Data_Image.xml(Default path is \ROT_Provisioning\OEMiROT\Images\OEMiROT_S_Data_Image.xml)
echo        Generate the data_enc_sign.bin image
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul
if "%s_data_image_number%" == "0" (goto :no_s_data)
%stm32tpccli% -pb %s_data_xml% >> %provisioning_log% 2>>&1
if !errorlevel! neq 0 goto :step_error

%stm32tpccli% -pb %s_data_init_xml% >> %provisioning_log% 2>>&1
if !errorlevel! neq 0 goto :step_error
:no_s_data

if "%app_full_secure%" == "1" (goto :no_ns_data)
echo    * Data non secure generation (if Data non secure image is enabled)
echo        Select OEMiROT_NS_Data_Image.xml(Default path is \ROT_Provisioning\OEMiROT\Images\OEMiROT_NS_Data_Image.xml)
echo        Generate the data_enc_sign.bin image
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul
if "%ns_data_image_number%" == "0" (goto :no_ns_data)
%stm32tpccli% -pb %ns_data_xml% >> %provisioning_log% 2>>&1
if !errorlevel! neq 0 goto :step_error

%stm32tpccli% -pb %ns_data_init_xml% >> %provisioning_log% 2>>&1
if !errorlevel! neq 0 goto :step_error
:no_ns_data

:: ============================================================== Provisioning steps =======================================================
echo Step 3 : Provisioning

echo    * BOOT0 pin should be disconnected from VDD
echo        (STM32WBA65I-DK1: no jumper on JP3)
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

:: ============================================================ OEM2 key provisioning ======================================================
:: Get OEM2 key
set /p oem2_key=<"%oem2_password%"

set "action=Programming oem2 password"
%stm32programmercli% %connect_reset% -hardRst -lockRDP2 %oem2_key% >> %provisioning_log% 2>>&1
IF !errorlevel! NEQ 0 goto :step_error
echo    * Provisioning of OEM2 key Done
echo.

:: ================================================== Option Bytes and flash programming ===================================================
set "action=Programming the option bytes and flashing the images..."
set current_log_file=%ob_flash_log%
set "command=start /w /b call %ob_flash_programming% AUTO %RDP_level%"
echo    * %action%
%command% > %ob_flash_log%

set ob_flash_error=!errorlevel!
if %ob_flash_error% neq 0 goto :step_error

echo        Successful option bytes programming and images flashing
echo        (see %ob_flash_log% for details)
echo.

:: =========================================================== RDP level setting ===========================================================
:set_rdp_level
set "action=Setting RDP level %RDP_level%"
set current_log_file=%provisioning_log%
echo    * %action%
%stm32programmercli% %connect_no_reset% -ob RDP=%rdp_value% >> %provisioning_log% 2>>&1
echo.
if "%rdp_value%" == "0xAA" ( goto final_execution )
echo    * Please unplug USB cable and plug it again to recover SWD Connection.
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul
:: In RDP 2, the connection with the board is lost and the return value of the command cannot be verified

:: ============================================================= End functions =============================================================
:final_execution
echo =====
echo ===== The board is correctly configured.
echo ===== Connect UART console (115200 baudrate) to get application menu.
echo =====
cmd /k
exit 0

:: Error when external script is executed
:step_error
echo.
echo =====
echo ===== Error while executing "%action%".
echo ===== See %current_log_file% for details. Then try again.
echo =====
cmd /k
exit 1
