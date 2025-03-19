@ECHO OFF
:: Enable delayed expansion
setlocal EnableDelayedExpansion
set "projectdir=%~dp0"
pushd %projectdir%\..\..\..\..\ROT_Provisioning
set provisioningdir=%cd%
popd

call %provisioningdir%/env.bat
call %provisioningdir%/OEMiROT/img_config.bat

IF "%oemurot_enabled%" == "1" (
set project=OEMuROT
set bootpath=OEMiRoT_OEMuRoT

call %provisioningdir%/OEMiRoT_OEMuRoT/img_config.bat
) else (
set project=OEMiROT
set bootpath=OEMiROT
)

:: Environment variable for log file
set current_log_file="%projectdir%\postbuild.log"
echo. > %current_log_file%

set "preprocess_bl2_file=%projectdir%\image_macros_preprocessed_bl2.c"

set "appli_dir=..\..\..\..\%oemirot_appli_path_project%"
set "postbuild_appli=%appli_dir%\EWARM\postbuild.bat"
set appli_flash_layout="%appli_dir%\Secure_nsclib\appli_flash_layout.h"
set loader_flash_layout="%projectdir%\..\..\..\..\%oemirot_loader_trustzone_path_project%\Secure_nsclib\loader_flash_layout.h"
set "postbuild_loader=%projectdir%\..\..\..\..\%oemirot_loader_trustzone_path_project%\EWARM\postbuild.bat"

set "ob_flash_programming=%provisioningdir%\%bootpath%\ob_flash_programming.bat"
set "img_config=%provisioningdir%\%bootpath%\img_config.bat"
set "regression=%provisioningdir%\%bootpath%\regression.bat"

::=================================================================================================
::image xml configuration files
::=================================================================================================
set s_code_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Code_Image.xml"
set ns_code_xml="%provisioningdir%\%bootpath%\Images\%project%_NS_Code_Image.xml"
set s_code_init_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Code_Init_Image.xml"
set ns_code_init_xml="%provisioningdir%\%bootpath%\Images\%project%_NS_Code_Init_Image.xml"
set s_data_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Data_Image.xml"
set ns_data_xml="%provisioningdir%\%bootpath%\Images\%project%_NS_Data_Image.xml"
set s_data_init_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Data_Init_Image.xml"
set ns_data_init_xml="%provisioningdir%\%bootpath%\Images\%project%_NS_Data_Init_Image.xml"

IF "%oemurot_enabled%" == "1" (
set oemurot_code_xml="%provisioningdir%\OEMiRoT_OEMuRoT\Images\OEMiROT_Code_Image.xml"
set oemurot_code_init_xml="%provisioningdir%\OEMiRoT_OEMuRoT\Images\OEMiROT_Code_Init_Image.xml"
)

::=================================================================================================
:: Variables for image xml configuration(ROT_Provisioning\%bootpath%\Images)
:: relative path from ROT_Provisioning\%bootpath%\Images directory to retrieve binary files
::=================================================================================================
set auth_s_xml_field="Authentication secure key"
set auth_ns_xml_field="Authentication non secure key"
set scratch_sector_number_xml_field="Number of scratch sectors"

:: Environment variable for AppliCfg
goto exe:
goto py:
:exe
::line for window executable
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
if exist %applicfg% (
echo run config Appli with windows executable
goto postbuild
)
:py
::line for python
echo run config Appli with python script
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python= "

:postbuild
:: =============================================================== Generate OEMuROT image ===============================================================
IF "%oemurot_enabled%" == "1" (
echo.
echo Creating OEMuROT image
echo.
echo Creating OEMuROT image  >> %current_log_file% 2>>&1
%stm32tpccli% -pb %oemurot_code_xml% >> %current_log_file% 2>>&1
IF !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %oemurot_code_init_xml% >> %current_log_file% 2>>&1
IF !errorlevel! neq 0 goto :error
)

:: ============================================================ Update %img_config% ============================================================
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b first_boot_stage -m RE_OEMIROT_FIRST_BOOT_STAGE --decimal --vb %img_config% >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b app_image_number -m RE_APP_IMAGE_NUMBER --decimal --vb %img_config% >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b s_data_image_number -m RE_S_DATA_IMAGE_NUMBER --decimal --vb %img_config% >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b ns_data_image_number -m RE_NS_DATA_IMAGE_NUMBER --decimal --vb %img_config% >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b app_full_secure -m  RE_OEMIROT_APPLI_FULL_SECURE --decimal --vb %img_config% >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

call %img_config%

IF "%first_boot_stage%" == "1" (
echo "OEMiROT_Boot is generated for OEMIROT_FIRST_BOOT_STAGE. Do not change any scripts!"
echo "copying ../Binary/OEMiROT_Boot.bin in %provisioningdir%/OEMiRoT_OEMuRoT/Binary/"
copy ..\Binary\OEMiROT_Boot.bin %provisioningdir%\OEMiRoT_OEMuRoT\Binary\  /b /y
exit 0
)

:: ============================================================ Update %regression% =====================================================================
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b secbootadd0 -m  RE_BL2_BOOT_ADDRESS  -d 0x80 %regression% --vb > %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b flashsectnbr -m  RE_FLASH_PAGE_NBR %regression% --vb >> %current_log_file% 2>>&1"
%command%
IF %errorlevel% NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b select_wba_target -m  RE_SELECT_WBA_TARGET --vb %regression% >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: ============================================================ Update %ob_flash_programming% ============================================================
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b keyaddress -m  RE_BL2_PERSO_ADDRESS %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b bootaddress -m  RE_BL2_BOOT_ADDRESS %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b oemirot_oemurot_slot0 -m  RE_OEMIROT_DOWNLOAD_IMAGE_AREA_0 %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b secbootadd0 -m RE_BL2_BOOT_ADDRESS -d 0x80 %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b loaderaddress -m RE_IMAGE_FLASH_LOADER_ADDRESS %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b cfg_loader -m RE_EXT_LOADER --decimal %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b flashsectnbr -m RE_FLASH_PAGE_NBR %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF %errorlevel% NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b slot0 -m RE_IMAGE_FLASH_ADDRESS_SECURE %ob_flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b slot1 -m RE_IMAGE_FLASH_ADDRESS_NON_SECURE %ob_flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b slot2 -m RE_IMAGE_FLASH_SECURE_UPDATE %ob_flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b slot3 -m RE_IMAGE_FLASH_NON_SECURE_UPDATE %ob_flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b slot4 -m RE_IMAGE_FLASH_ADDRESS_DATA_SECURE %ob_flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b slot5 -m RE_IMAGE_FLASH_ADDRESS_DATA_NON_SECURE %ob_flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b slot6 -m RE_IMAGE_FLASH_DATA_SECURE_UPDATE %ob_flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b slot7 -m RE_IMAGE_FLASH_DATA_NON_SECURE_UPDATE %ob_flash_programming% --vb >> %current_log_file% 2>&1"
%command%
IF %ERRORLEVEL% NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b wrp_a_bank1_start -m RE_BL2_WRP_START -d 0x2000 %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b wrp_a_bank1_end -m RE_BL2_WRP_END -d 0x2000 %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b sec1_start -m RE_BL2_SEC1_START -d 0x1000 %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b sec1_end -m RE_BL2_SEC1_END -d 0x2000 %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b hdp_end -m RE_BL2_HDP_END -d 0x2000 %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b wrp_a_bank2_start -m RE_LOADER_WRP_START -d 0x2000 %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b wrp_a_bank2_end -m RE_LOADER_WRP_END -d 0x2000 %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b external_flash -m RE_EXTERNAL_FLASH_ENABLE --decimal %ob_flash_programming% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b select_wba_target -m  RE_SELECT_WBA_TARGET --vb %ob_flash_programming% >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

IF "%oemurot_enabled%" NEQ "1" (goto no_second_boot_stage)
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b is_wba_board_dk -m  RE_IS_WBA_BOARD_DK --decimal --vb %ob_flash_programming% >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error
:no_second_boot_stage

:: ============================================================ Update %postbuild_appli% ============================================================
IF "%app_full_secure%" == "1" (goto bypass_postbuild_appli_update)
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b image_s_size -m  RE_IMAGE_FLASH_SECURE_IMAGE_SIZE %postbuild_appli% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b image_ns_size -m  RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE %postbuild_appli% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b app_image_number -m  RE_APP_IMAGE_NUMBER --decimal %postbuild_appli% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error
:bypass_postbuild_appli_update

:: ============================================================ Update %ns_code_xml% ============================================================
IF "%app_full_secure%" == "1" (goto bypass_ns_code_xml_update)
set "command=%python%%applicfg% xmlname --layout %preprocess_bl2_file% -m RE_APP_IMAGE_NUMBER -n %auth_ns_xml_field% -sn %auth_s_xml_field% -v 1 -c k %ns_code_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE -c S %ns_code_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" %ns_code_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %ns_code_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE -c S %ns_code_init_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error
:bypass_ns_code_xml_update

:: ============================================================ Update %s_code_xml% ============================================================
set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -c S %s_code_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" %s_code_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %s_code_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -c S %s_code_init_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: ============================================================ Update %ns_data_xml% ============================================================
IF "%app_full_secure%" == "1" (goto bypass_ns_data_xml_update)
set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -c S %ns_data_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" %ns_data_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %ns_data_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -c S %ns_data_init_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error
:bypass_ns_data_xml_update

:: ============================================================ Update %s_data_xml% ============================================================
set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -c S %s_data_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" %s_data_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlparam --layout  %preprocess_bl2_file% -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" %s_data_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% xmlval --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -c S %s_data_init_xml% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: ============================================================ Update %appli_flash_layout% ============================================================
set "command=%python%%applicfg% setdefine --layout %preprocess_bl2_file% -m RE_OVER_WRITE -n MCUBOOT_OVERWRITE_ONLY -v 1 %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_APP_IMAGE_NUMBER -n MCUBOOT_APP_IMAGE_NUMBER %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_S_DATA_IMAGE_NUMBER -n MCUBOOT_S_DATA_IMAGE_NUMBER %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_DATA_IMAGE_NUMBER -n MCUBOOT_NS_DATA_IMAGE_NUMBER %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n FLASH_AREA_0_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_SIZE -n FLASH_AREA_0_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_1_OFFSET -n FLASH_AREA_1_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_1_SIZE -n FLASH_AREA_1_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_2_OFFSET -n FLASH_AREA_2_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_2_SIZE -n FLASH_AREA_2_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_3_OFFSET -n FLASH_AREA_3_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_3_SIZE -n FLASH_AREA_3_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_4_OFFSET -n FLASH_AREA_4_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_4_SIZE -n FLASH_AREA_4_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_5_OFFSET -n FLASH_AREA_5_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_5_SIZE -n FLASH_AREA_5_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_6_OFFSET -n FLASH_AREA_6_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_6_SIZE -n FLASH_AREA_6_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_7_OFFSET -n FLASH_AREA_7_OFFSET %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_7_SIZE -n FLASH_AREA_7_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_S_NS_PARTITION_SIZE -n FLASH_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE -n FLASH_NS_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n FLASH_S_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -n FLASH_S_DATA_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -n FLASH_NS_DATA_PARTITION_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_B_SIZE -n FLASH_B_SIZE %appli_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: ============================================================ Update %postbuild_loader% ============================================================
set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b loader_ns_size -m RE_LOADER_NS_CODE_SIZE %postbuild_loader% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% flash --layout %preprocess_bl2_file% -b loader_s_size -m RE_LOADER_IMAGE_S_CODE_SIZE %postbuild_loader% --vb >> %current_log_file% 2>>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: ============================================================ Update %loader_flash_layout% ============================================================
set "command=%python%%applicfg% setdefine --layout %preprocess_bl2_file% -m RE_OVER_WRITE -n MCUBOOT_OVERWRITE_ONLY -v 1 %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_APP_IMAGE_NUMBER -n MCUBOOT_APP_IMAGE_NUMBER %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_S_DATA_IMAGE_NUMBER -n MCUBOOT_S_DATA_IMAGE_NUMBER %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_NS_DATA_IMAGE_NUMBER -n MCUBOOT_NS_DATA_IMAGE_NUMBER %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_OFFSET -n FLASH_AREA_0_OFFSET %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_0_SIZE -n FLASH_AREA_0_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_1_OFFSET -n FLASH_AREA_1_OFFSET %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_1_SIZE -n FLASH_AREA_1_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_2_OFFSET -n FLASH_AREA_2_OFFSET %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_2_SIZE -n FLASH_AREA_2_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_3_OFFSET -n FLASH_AREA_3_OFFSET %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_3_SIZE -n FLASH_AREA_3_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_4_OFFSET -n FLASH_AREA_4_OFFSET %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_4_SIZE -n FLASH_AREA_4_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_5_OFFSET -n FLASH_AREA_5_OFFSET %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_5_SIZE -n FLASH_AREA_5_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_6_OFFSET -n FLASH_AREA_6_OFFSET %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_6_SIZE -n FLASH_AREA_6_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_7_OFFSET -n FLASH_AREA_7_OFFSET %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_AREA_7_SIZE -n FLASH_AREA_7_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_S_NS_PARTITION_SIZE -n FLASH_PARTITION_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE -n FLASH_NS_PARTITION_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n FLASH_S_PARTITION_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -n FLASH_S_DATA_PARTITION_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -n FLASH_NS_DATA_PARTITION_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

set "command=%python%%applicfg% definevalue --layout %preprocess_bl2_file% -m RE_FLASH_B_SIZE -n FLASH_B_SIZE %loader_flash_layout% --vb >> %current_log_file% 2>&1"
%command%
IF !errorlevel! NEQ 0 goto :error

:: ============================================================ end ============================================================
exit 0

:: ============================================================ error ============================================================
:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo "%command% : failed" >> %current_log_file%
echo =====
exit 1

