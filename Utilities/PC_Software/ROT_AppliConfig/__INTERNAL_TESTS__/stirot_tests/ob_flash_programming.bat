@echo off

IF [%1] NEQ [AUTO] call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR
set connect_normal=-c port=SWD speed=fast ap=1 mode=normal

set app_image_number=1

set STiROT_iLoader_bin="%stirot_iloader_boot_path_project%\Binary\STiROT_iLoader.bin"
set STiROT_iLoader_bin=%STiROT_iLoader_bin:\=/%
set OEMuROT_hex="%~dp0..\..\Applications\ROT\OEMiROT_Boot\Binary\OEMiROT_Boot_enc_sign.hex"
set oemurot_appli_hex="%oemirot_boot_path_project%\Binary\%oemirot_appli%"

:: =============================================== Remove protections and erase the user flash ===============================================

set erase_all=-e all
set "wrps_disable=0xff"
set "hdp_area_start=0x0ff"
set "hdp_area_end=0x000"

:: =============================================== Configure Option Bytes ====================================================================

set "action=Remove Protection and erase All"
echo    * %action%

:: Disable WRP protections
:: WRPS
%stm32programmercli% %connect_reset% >> nul
set ob_add=0x5200221c
set ob_mask=0x000000ff
set ob_pos=0x00
set "command=%stm32programmercli% %connect_no_reset% -obrss %ob_add% %ob_mask% %wrps_disable% %ob_pos%"
echo !command!
!command!
IF %errorlevel% NEQ 0 goto :error

:: Disable HDP protection on user flash
:: HDP_AREA_START
%stm32programmercli% %connect_reset% >> nul
set ob_add=0x52002230
set ob_mask=0x000003ff
set ob_pos=0x00
set "command=%stm32programmercli% %connect_no_reset% -obrss %ob_add% %ob_mask% %hdp_area_start% %ob_pos%"
echo !command!
!command!
IF %errorlevel% NEQ 0 goto :error

:: HDP_AREA_END
%stm32programmercli% %connect_reset% >> nul
set ob_add=0x52002230
set ob_mask=0x000003ff
set ob_pos=0x10
set "command=%stm32programmercli% %connect_no_reset% -obrss %ob_add% %ob_mask% %hdp_area_end% %ob_pos%"
echo !command!
!command!
IF %errorlevel% NEQ 0 goto :error

%stm32programmercli% %connect_reset% >> nul
%stm32programmercli% %connect_no_reset% %erase_all%
IF %errorlevel% NEQ 0 goto :error

set "action=Erase external non volatile primary code slot"
echo %action%
set ext_nvm_primary_code_start=0
set ext_nvm_primary_code_stop=1
set "command=%stm32programmercli% -c port=%com_port% br=921600 %stm32ExtLoaderFlash% -e [%ext_nvm_primary_code_start% %ext_nvm_primary_code_stop%]"
echo !command!
!command!
IF %errorlevel% NEQ 0 goto :error

echo "action=Set SRAM ECC configuration"
echo %action%
:: Recommended configuration for secure boot is :
::   - SRAM ECC activated. Hack tentative detection enabled ==> ECC_ON_SRAM=2
%stm32programmercli% %connect_no_reset% -ob ECC_ON_SRAM=(2) #toto
IF %errorlevel% NEQ 0 goto :error

:: ================================================== Download STiROT_iLoader ==================================================================
IF not exist %STiROT_iLoader_bin% (
    @echo [31mError: ST_iROT_iLoader.bin does not exist! [0m
    goto :error
)
set OEMiLoader_address=0x6000000

if [%1] neq [AUTO] pause >nul
%stm32programmercli% %connect_no_reset% -d %STiROT_iLoader_bin% %OEMiLoader_address%
IF !errorlevel! NEQ 0 goto :error

set "action=Define write protected area"
echo %action%
:: This configuration depends on user mapping but the watermarks should cover at least the STiROT iloader.
set wrps0=0
set wrps1=0
set wrps2=0
set wrps3=1
set wrps4=1
set wrps5=1
set wrps6=1
set wrps7=1
set /A "wrps=(%wrps0%<<0)|(%wrps1%<<1)|(%wrps2%<<2)|(%wrps3%<<3)|(%wrps4%<<4)|(%wrps5%<<5)|(%wrps6%<<6)|(%wrps7%<<7)"

:: WRPS
set ob_add=0x5200221c
set ob_mask=0x000000ff
set ob_pos=0x00
set "command=%stm32programmercli% %connect_no_reset% -obrss %ob_add% %ob_mask% %wrps% %ob_pos%"
echo !command!
!command!
IF %errorlevel% NEQ 0 goto :error

%stm32programmercli% %connect_reset% >> nul


:: ==================================================== Download OEMuROT ====================================================================
set "action=Download the STiROT OEMuROT image in the download slots"
echo %action%

IF not exist %OEMuROT_hex% (
    @echo [31mError: OEMiROT_Boot_enc_sign.hex does not exist! use TPC to generate it[0m
    goto :error
)
%stm32programmercli% -c port=%com_port% br=921600 %stm32ExtLoaderFlash% -d %OEMuROT_hex%
IF !errorlevel! NEQ 0 goto :error

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
    %stm32programmercli% -c port=%com_port% br=921600 %stm32ExtLoaderFlash% -d %oemurot_appli_hex%
    IF !errorlevel! NEQ 0 goto :error

    echo "Appli Written"
)

:: ==================================================== DTCM_AXI_SHARE ====================================================================

:: Place to change DTCM_AXI_SHARE configurations if there is a need
::set "command=%stm32programmercli% %connect_no_reset% -ob DTCM_AXI_SHARE=0
::echo !command!
::!command!

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
