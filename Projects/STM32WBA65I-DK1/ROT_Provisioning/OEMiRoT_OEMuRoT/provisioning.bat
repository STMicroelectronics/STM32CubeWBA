@ECHO OFF
:: Getting the CubeProgammer_cli path
call ../env.bat
set cube_fw_path=%cube_fw_path:"=%

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set "projectdir=%~dp0"

:: Environment variable used to generate binary keys
set oemurot_config_xml="%projectdir%\Config\OEMuRoT_Config.xml"
set oemirot_config_xml="%projectdir%\Config\OEMiRoT_Config.xml"

:: Path of files to be updated in OEMiROT_Boot
set "flash_layout=%cube_fw_path%\Projects\STM32WBA65I-DK1\Applications\ROT\OEMiROT_Boot\Inc\flash_layout.h"
set boot_cfg_h="%cube_fw_path%\Projects\STM32WBA65I-DK1\Applications\ROT\OEMiROT_Boot\Inc\boot_hal_cfg.h"

:: CubeProgammer path and input files
set ob_flash_programming="ob_flash_programming.bat"

:: OEM2 key path
set oem2_password="./Keys/oem2_password.txt"

:: Log files
set ob_flash_log="ob_flash_programming.log"
set provisioning_log="provisioning.log"
del -rf *.log

goto exe:
goto py:
:exe
::line for window executable
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %applicfg% (
echo run config Appli with windows executable
goto prov
)
:py
::line for python
echo run config Appli with python script
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python=python "

:prov
:: Display Boot path
echo =====
echo ===== Provisioning of OEMiROT_OEMuRoT boot path
echo ===== Application selected through env.bat:
echo ===== %oemirot_appli_path_project%
echo =====
echo.

:: Get config updated by OEMiROT_Boot
set tmp_oemurot_file=%projectdir%\img_config.bat
set tmp_oemirot_file=%projectdir%\..\OEMiROT\img_config.bat

:: ====================================== Step to update oemurot value in img_config.bat =========================================
:: update oemurot value in img_config.bat
set "command=%applicfg% definevalue --name=oemurot_enabled  --value=1 %tmp_oemirot_file% --decimal"
%command%
if !errorlevel! neq 0 goto :step_error

:: Get config updated by OEMiROT_Boot
set appli_dir=..\..\%oemirot_appli_path_project%
set boot_dir=..\..\%oemirot_boot_path_project%

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

set fw_in_bin="Firmware binary input file"
set fw_out_bin="Image output file"
set ns_app_bin="..\%appli_dir%\Binary\oemirot_tz_ns_app.bin"
set s_app_bin="..\%appli_dir%\Binary\oemirot_tz_s_app.bin"
set s_code_image_file="%projectdir%Images\OEMuROT_S_Code_Image.xml"
set ns_code_image_file="%projectdir%Images\OEMuROT_NS_Code_Image.xml"
set s_data_xml="%projectdir%Images\OEMuROT_S_Data_Image.xml"
set ns_data_xml="%projectdir%Images\OEMuROT_NS_Data_Image.xml"
set s_data_init_xml="%projectdir%Images\OEMuROT_S_Data_Init_Image.xml"
set ns_data_init_xml="%projectdir%Images\OEMuROT_NS_Data_Init_Image.xml"
set ns_app_enc_sign_bin="../%appli_dir%/Binary/%oemurot_appli_non_secure%"
set s_app_enc_sign_bin="../%appli_dir%/Binary/%oemurot_appli_secure%"

:: Initial configuration
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD mode=UR ap=1

:: ========================================================= Configuration management========================================================
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
echo        Open oem2_password file and put OEM2 key(Default path is \ROT_Provisioning\OEMiRoT_OEMuRoT\Keys\oem2_password.txt)
echo        Warning: Default OEM2 keys must NOT be used in a product. Make sure to regenerate your own OEM2 keys!
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

:no_oem2_key
:: Step to update RDP Level in boot_hal_cfg
:: Replace RDP level in boot_hal_cfg
set "action=Update RDP Level in boot_hal_cfg"
%python%%applicfg% modifyfilevalue --variable OEMIROT_OB_RDP_LEVEL_VALUE --value %rdp_str% %boot_cfg_h% --str

:: ================================================ Step to generate OEMiRoT_Keys.bin file =================================================
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

:: ================================================ Step to generate OEMuRoT_Keys.bin file =================================================
echo    * OEMuROT keys configuration
echo        From TrustedPackageCreator (OBkey tab in Security panel)
echo        Select OEMuRoT_Config.xml(Default path is /ROT_Provisioning/OEMiRoT_OEMuRoT/Config/OEMuRoT_Config.xml)
echo        Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys!
echo        Update the configuration (if/as needed) then generate OEMuRoT_Keys.bin file
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul
%stm32tpccli% -obk %oemurot_config_xml% >> %provisioning_log% 2>>&1
if !errorlevel! neq 0 goto :step_error

:: ======================================== Step Configure OEMIROT_Boot project as OEMUROT =================================================
set "command=%python%%applicfg% setdefine -a uncomment -n OEMUROT_ENABLE -v 1 %flash_layout%"
%command%
IF !errorlevel! NEQ 0 goto :step_error

:: ========================================================= Images generation steps =======================================================
echo Step 2 : Images generation
echo    * OEMiROT Boot firmware image
echo      The OEMiROT_Boot binary is %oemirot_boot_path_binary%
echo.
echo    * OEMuROT Boot firmware image generation
echo        Open the OEMiROT_Boot project with preferred toolchain and rebuild all files.
echo        At this step the project is configured for OEMIROT_OEMuROT boot path as OEMuRoT.
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

call %tmp_oemurot_file%
if  "%app_image_number%" == "2" goto :next
if  "%app_full_secure%" == "1" (
set s_app_enc_sign_bin="../%appli_dir%/Binary/%oemurot_appli_secure%"
set s_app_bin="..\%appli_dir%\Binary\oemirot_tz_s_app.bin"
) else (
set ns_app_enc_sign_bin="../%appli_dir%/Binary/%oemurot_appli_assembly_sign%"
set ns_app_bin="..\%appli_dir%\Binary\oemirot_tz_app.bin"
)
:next
set "command=%python%%applicfg% xmlval -v %s_app_bin% --string -n %fw_in_bin% %s_code_image_file%"
%command%
IF !errorlevel! NEQ 0 goto :step_error
set "command=%python%%applicfg% xmlval -v %ns_app_bin% --string -n %fw_in_bin% %ns_code_image_file%"
%command%
IF !errorlevel! NEQ 0 goto :step_error
set "command=%python%%applicfg% xmlval -v %s_app_enc_sign_bin% --string -n %fw_out_bin% %s_code_image_file%"
%command%
IF !errorlevel! NEQ 0 goto :step_error
set "command=%python%%applicfg% xmlval -v %ns_app_enc_sign_bin% --string -n %fw_out_bin% %ns_code_image_file%"
%command%
IF !errorlevel! NEQ 0 goto :step_error

echo    * Code firmware image generation
IF "%app_full_secure%" == "1" (
echo        Open the OEMiROT_Appli project with preferred toolchain.
echo        Rebuild the Secure project. The %oemurot_appli_secure% and oemurot_tz_s_app_enc_sign.bin files are generated with the postbuild command.
) else (
echo        Open the OEMiROT_Appli_TrustZone project with preferred toolchain.
echo        Rebuild the Secure project. The %oemurot_appli_secure% and oemurot_tz_s_app_enc_sign.bin files are generated with the postbuild command.
echo        Rebuild the NonSecure project. The %oemurot_appli_non_secure% and oemurot_tz_ns_app_enc_sign.bin files are generated with the postbuild command.
)
echo        Press any key to continue...
echo.
if [%1] neq [AUTO] pause >nul

echo    * Data secure generation (if Data secure image is enabled)
echo        Select OEMuRoT_S_Data_Image.xml(Default path is \ROT_Provisioning\OEMiRoT_OEMuRoT\Images\OEMuROT_S_Data_Image.xml)
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

echo    * Data non secure generation (if Data non secure image is enabled)
echo        Select OEMuROT_NS_Data_Image.xml(Default path is \ROT_Provisioning\OEMiRoT_OEMuRoT\Images\OEMuROT_NS_Data_Image.xml)
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
