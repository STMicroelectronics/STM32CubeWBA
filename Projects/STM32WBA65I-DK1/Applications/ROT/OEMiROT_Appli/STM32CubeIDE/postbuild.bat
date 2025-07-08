@ECHO OFF
:: arg1 is the security type (nonsecure, secure)
:: When script is called from STM32CubeIDE : set signing="%1"
:: When script is called from IAR or KEIL  : set "signing=%1"
set signing="%1"

:: Getting the Trusted Package Creator CLI path
set "projectdir=%~dp0"
::echo %projectdir%
pushd %projectdir%\..\..\..\..\ROT_Provisioning
set provisioningdir=%cd%

popd
call "%provisioningdir%\env.bat"
call %provisioningdir%/OEMiROT/img_config.bat

IF "%oemurot_enabled%" == "1" (
set project=OEMuROT
set bootpath=OEMiRoT_OEMuRoT
) else (
set project=OEMiROT
set bootpath=OEMiROT
)

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Environment variable for log file
set current_log_file="%projectdir%\postbuild.log"
echo. > %current_log_file%

::=================================================================================================
::image xml configuration files
::=================================================================================================
set s_code_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Code_Image.xml"
set s_code_init_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Code_Init_Image.xml"
set s_data_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Data_Image.xml"
set s_data_init_xml="%provisioningdir%\%bootpath%\Images\%project%_S_Data_Init_Image.xml"

::=================================================================================================
:: Variables for image xml configuration(ROT_Provisioning\%bootpath%\Images)
:: relative path from ROT_Provisioning\%bootpath%\Images directory to retrieve binary files
::=================================================================================================
set bin_path_xml_field="..\..\..\Applications\ROT\OEMiROT_Appli\Binary"
set data_path_xml_field="..\Binary"
set fw_in_bin_xml_field="Firmware binary input file"
set fw_out_bin_xml_field="Image output file"
set s_app_bin_xml_field="%bin_path_xml_field%\oemirot_tz_s_app.bin"
set s_data_enc_sign_bin_xml_field="%data_path_xml_field%\s_data_enc_sign.bin"
set s_data_init_sign_bin_xml_field="%data_path_xml_field%\s_data_init_sign.bin"

IF "%oemurot_enabled%" == "1" (
set s_app_enc_sign_bin_xml_field="%bin_path_xml_field%\oemurot_tz_s_app_enc_sign.bin"
set s_app_init_sign_bin_xml_field="%bin_path_xml_field%\%oemurot_appli_secure%"
) else (
set s_app_enc_sign_bin_xml_field="%bin_path_xml_field%\oemirot_tz_s_app_enc_sign.bin"
set s_app_init_sign_bin_xml_field="%bin_path_xml_field%\%oemirot_appli_secure%"
)

:start
goto exe:
goto py:
:exe
::line for window executable
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\dist\AppliCfg.exe"
set "python="
IF exist %applicfg% (
echo Postbuild with windows executable
goto postbuild
)
:py
::called if we just want to use AppliCfg python (think to comment "goto exe:")
echo Postbuild with python script
set "applicfg=%cube_fw_path%\Utilities\PC_Software\ROT_AppliConfig\AppliCfg.py"
set "python=python "

:postbuild
echo Postbuild %signing% image >> %current_log_file% 2>>&1

::update xml file : input file
%python%%applicfg% xmlval -v %s_app_bin_xml_field% --string -n %fw_in_bin_xml_field% %s_code_xml% --vb >> %current_log_file% 2>>&1
IF !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %s_app_enc_sign_bin_xml_field% --string -n %fw_out_bin_xml_field% %s_code_xml% --vb >> %current_log_file% 2>>&1
IF !errorlevel! neq 0 goto :error

echo Creating secure image  >> %current_log_file% 2>>&1
%stm32tpccli% -pb %s_code_xml% >> %current_log_file% 2>>&1
IF !errorlevel! neq 0 goto :error

::update xml file : input file
%python%%applicfg% xmlval -v %s_app_bin_xml_field% --string -n %fw_in_bin_xml_field% %s_code_init_xml% --vb >> %current_log_file% 2>>&1
IF !errorlevel! neq 0 goto :error

::update xml file : output file
%python%%applicfg% xmlval -v %s_app_init_sign_bin_xml_field% --string -n %fw_out_bin_xml_field% %s_code_init_xml% --vb >> %current_log_file% 2>>&1
IF !errorlevel! neq 0 goto :error

%stm32tpccli% -pb %s_code_init_xml% >> %current_log_file% 2>>&1
IF !errorlevel! neq 0 goto :error

::update data xml file : output file
%python%%applicfg% xmlval -v %s_data_enc_sign_bin_xml_field% --string -n %fw_out_bin_xml_field% %s_data_xml% --vb >> %current_log_file% 2>>&1
IF !errorlevel! neq 0 goto :error

%python%%applicfg% xmlval -v %s_data_init_sign_bin_xml_field% --string -n %fw_out_bin_xml_field% %s_data_init_xml% --vb >> %current_log_file% 2>>&1
IF !errorlevel! neq 0 goto :error

exit 0

:error
echo.
echo =====
echo ===== Error occurred.
echo ===== See %current_log_file% for details. Then try again.
echo =====
exit 1
