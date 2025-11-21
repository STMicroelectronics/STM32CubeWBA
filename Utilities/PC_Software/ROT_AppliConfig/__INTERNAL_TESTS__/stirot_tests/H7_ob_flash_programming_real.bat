@echo off

IF [%1] NEQ [AUTO] call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR
set connect_normal=-c port=SWD speed=fast ap=1 mode=normal

set image_number=2
set app_image_number=1

set OEMiLoader_bin="%OEMiLoader_boot_path_project%\Binary\STiROT_iLoader.bin"
set OEMiLoader_bin=%OEMiLoader_bin:\=/%
set OEMuROT_hex="%~dp0..\..\Applications\ROT\OEMiROT_Boot\Binary\OEMiROT_Boot_enc_sign.hex"
set oemurot_appli_hex="%oemirot_boot_path_project%\Binary\%oemirot_appli%"

:: =============================================== Remove protections and erase the user flash ===============================================

set remove_protect=-ob WRPS0=1 WRPS1=1 WRPS2=1 WRPS3=1 WRPS4=1 WRPS5=1 WRPS6=1 WRPS7=1 HDP_AREA_START=1 HDP_AREA_END=0 ITCM_AXI_SHARE=0 DTCM_AXI_SHARE=0
set erase_all=-e all

:: =============================================== Configure Option Bytes ====================================================================

set "action=Remove Protection and erase All"
echo %action%
%stm32programmercli% %connect_reset% %remove_protect% %erase_all%
IF %errorlevel% NEQ 0 goto :error

set "action=Erase external non volatile primary code slot"
echo %action%
set ext_nvm_primary_code_start=0
set ext_nvm_primary_code_stop=0
%stm32programmercli% %connect_normal% %stm32ExtLoaderFlash% -e ^[%ext_nvm_primary_code_start% %ext_nvm_primary_code_stop%^]
IF %errorlevel% NEQ 0 goto :error

echo "Set SRAM ECC configuration"
:: Recommended configuration for secure boot is :
::   - SRAM ECC activated. Hack tentative detection enabled ==> ECC_ON_SRAM=1
%stm32programmercli% %connect_no_reset% -ob ECC_ON_SRAM=1
IF !errorlevel! NEQ 0 goto :error

:: ================================================== Download OEMiLoader ==================================================================
IF not exist %OEMiLoader_bin% (
    @echo [31mError: ST_iROT_iLoader.bin does not exist! [0m
    goto :error
)
set OEMiLoader_address=0x8000000
if [%1] neq [AUTO] pause >nul
%stm32programmercli% %connect_no_reset% -d %OEMiLoader_bin% %OEMiLoader_address%
IF !errorlevel! NEQ 0 goto :error

set "action=Define write protected area"
echo %action%
:: This configuration depends on user mapping but the watermarks should cover at least the secure area part of the firmware execution slot.
:: The secure area can also be larger in order to include additional sectors. For example the secure firmware will have to manage user data.
set wrps0=0
set wrps1=0
set wrps2=0
set wrps3=1
set wrps4=1
set wrps5=1
set wrps6=1
set wrps7=1
%stm32programmercli% %connect_no_reset% -ob WRPS0=%wrps0% WRPS1=%wrps1% WRPS2=%wrps2% WRPS3=%wrps3% WRPS4=%wrps4% WRPS5=%wrps5% WRPS6=%wrps6% WRPS7=%wrps7%

IF !errorlevel! NEQ 0 goto :error

:: ==================================================== Download OEMuROT ====================================================================
set "action=Download the STiROT OEMuROT image in the download slots"
echo %action%

IF not exist %OEMuROT_hex% (
    @echo [31mError: OEMiROT_Boot_enc_sign.hex does not exist! use TPC to generate it[0m
    goto :error
)
%stm32programmercli% %connect_normal% -d %OEMuROT_hex% %stm32ExtLoaderFlash%
IF !errorlevel! NEQ 0 goto :error

if  "%image_number%" == "2" (
    set "action=Download the data image in the download slots"
    echo %action%
    IF not exist %~dp0.\Binary\data_enc_sign.hex (
        @echo [31mError: data_enc_sign.hex does not exist! use TPC to generate it[0m
        goto :error
    )
    %stm32programmercli% %connect_normal% -d %~dp0.\Binary\data_enc_sign.hex %stm32ExtLoaderFlash%
    IF !errorlevel! NEQ 0 goto :error
)

:: ==================================================== Download images ====================================================================
set "action=OEMuROT application images programming in download slots"
echo %action%

if  "%app_image_number%" == "1" (
    set "action=Write OEMiROT_Appli Code"
    echo !action!

    IF not exist %oemurot_appli_hex% (
        @echo [31mError: %oemurot_appli_hex% does not exist! use TPC to generate it[0m
        goto :error
    )
    %stm32programmercli% %connect_normal% -d %oemurot_appli_hex% %stm32ExtLoaderFlash%
    IF !errorlevel! NEQ 0 goto :error

    echo "Appli Written"
)

echo Programming success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo      Error when trying to "%action%" >CON
echo      Programming aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1

:dectohex
exit /b %1
