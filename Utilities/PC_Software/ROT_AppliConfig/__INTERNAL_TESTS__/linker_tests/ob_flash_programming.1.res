@echo off

IF [%1] NEQ [AUTO] call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Data updated with the postbuild of OEMiROT-Boot
set wrps0=0
set wrps1=0
set wrps2=0
set wrps3=0
set wrps4=0
set wrps5=0
set wrps6=0
set wrps7=0
set bootaddress=0x8000000
set hdp_start=0x0
set hdp_end=0xFF
set app_image_number=1
set code_image=%oemirot_appli%
set oemirot_code="%~dp0..\..\Applications\ROT\OEMiROT_Boot\Binary\OEMiROT_Boot.bin"

:: CubeProgammer connection
set connect_no_reset=-c port=SWD speed=fast ap=1 mode=Hotplug
set connect_reset=-c port=SWD speed=fast ap=1 mode=UR
set connect_normal=-c port=SWD speed=fast ap=1 mode=normal

:: =============================================== Remove protections and erase the user flash ===============================================

set remove_protect=-ob WRPS0=1 WRPS1=1 WRPS2=1 WRPS3=1 WRPS4=1 WRPS5=1 WRPS6=1 WRPS7=1 HDP_AREA_START=1 HDP_AREA_END=0 ITCM_AXI_SHARE=0 DTCM_AXI_SHARE=0
set erase_all=-e all

:: =============================================== Configure Option Bytes ====================================================================

set "action=Remove Protection and erase All"
echo %action%
%stm32programmercli% %connect_reset% %remove_protect% %erase_all%
IF !errorlevel! NEQ 0 goto :error

echo "Set SRAM ECC configuration"
:: Recommended configuration for secure boot is :
::   - SRAM ECC activated. Hack tentative detection enabled ==> ECC_ON_SRAM=1
%stm32programmercli% %connect_no_reset% -ob ECC_ON_SRAM=1
IF %errorlevel% NEQ 0 goto :error

:: ================================================== Download OEMiROT_Boot ==================================================================

set "action=Write OEMiROT_Boot"
echo %action%
%stm32programmercli% %connect_no_reset% -d %oemirot_code% %bootaddress% -v
IF !errorlevel! NEQ 0 goto :error

set "action=OEMiROT_Boot Written"
echo %action%

:: ======================================================= Extra board protections =========================================================

set "action=Define write protected area and hide protected area"
echo %action%
%stm32programmercli% %connect_no_reset% -ob WRPS0=%wrps0% WRPS1=%wrps1% WRPS2=%wrps2% WRPS3=%wrps3% WRPS4=%wrps4% WRPS5=%wrps5% WRPS6=%wrps6% WRPS7=%wrps7% HDP_AREA_START=%hdp_start% HDP_AREA_END=%hdp_end%

IF %errorlevel% NEQ 0 goto :error

:: ==================================================== Download images ====================================================================
set "action=Application images programming in download slots"
echo %action%

if  "%app_image_number%" == "1" (
    set "action=Write OEMiROT_Appli Code"
    echo %action%
    %stm32programmercli% %connect_normal% -d %oemirot_boot_path_project%\Binary\%code_image% %stm32ExtLoaderFlash%
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

