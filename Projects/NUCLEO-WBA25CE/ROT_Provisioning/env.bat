@ECHO OFF

:: ==============================================================================
::                            General
:: ==============================================================================
:: Configure tools installation path
set stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
set stm32tpccli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32TrustedPackageCreator_CLI.exe"
set imgtool="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\Utilities\Windows\imgtool.exe"

:: ==============================================================================
::                            OEMiROT Appli path
:: ==============================================================================
:: Select application project below
::set oemirot_appli_path_project=Templates\ROT\OEMiROT_Appli
::set oemirot_appli_path_project=Applications\ROT\OEMiROT_Appli
::set oemirot_appli_path_project=Templates\ROT\OEMiROT_Appli_TrustZone
set oemirot_appli_path_project=Applications\ROT\OEMiROT_Appli_TrustZone

:: ==============================================================================
::                            OEMiROT Boot path
:: ==============================================================================
:: Select OEMiROT Boot project below
set oemirot_boot_path_project=Applications\ROT\OEMiROT_Boot

:: ==============================================================================
::                            OEMiRoT_OEMuRoT boot path
:: ==============================================================================
:: Select oemirot binary
set oemirot_boot_path_binary=/ROT_Provisioning/OEMiRoT_OEMuRoT/Binary/OEMiROT_Boot.bin
:: oemurot project and application project are the same as for OEMiROT boot path

:: ==============================================================================

set cube_fw_path=%~dp0..\..\..\
set rot_provisioning_path="%~dp0"
set oemirot_appli_secure=oemirot_tz_s_app_init_sign.bin
set oemirot_appli_non_secure=oemirot_tz_ns_app_init_sign.bin
set oemirot_appli_assembly_sign=oemirot_tz_app_init_sign.bin
set oemurot_appli_secure=oemurot_tz_s_app_init_sign.bin
set oemurot_appli_non_secure=oemurot_tz_ns_app_init_sign.bin
set oemurot_appli_assembly_sign=oemurot_tz_app_init_sign.bin
