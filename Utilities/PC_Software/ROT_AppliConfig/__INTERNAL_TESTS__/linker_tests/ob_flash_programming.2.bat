call ../env.bat

:: Enable delayed expansion
setlocal EnableDelayedExpansion

:: Data updated with the postbuild of OEMiROT-Boot
set wrp1a_end=0x11FFF
set wrp1b_start=0x1C000
set wrp1b_end=0x20000
set hdp_end=0x1BFFF
set appliaddress=0x8030000
set dataaddress=0x803E000
set loaderaddress=0x801C000
set code_image=%oemirot_appli%

:: STM32CubeProgammer connection
set connect_no_reset=-c port=SWD mode=Hotplug
set connect_reset=-c port=SWD mode=UR

:: =============================================== Remove protections and initialize Option Bytes  ==========================================
set remove_protect_init=-ob WRP1A_STRT=0x7f WRP1A_END=0 WRP1B_STRT=0x7f WRP1B_END=0 HDP1_PEND=0 HDP1EN=0xB4 BOOT_LOCK=0
:: =============================================== Erase the user flash =====================================================================
set erase_all=-e all
:: ================================================ hardening ===============================================================================
set hide_protect=HDP1_PEND=%hdp_end% HDP1EN=1
set boot_write_protect=WRP1A_STRT=0x7f WRP1A_END=%wrpgrp1%
set loader_write_protect=WRP1B_STRT=%wrpgrp2% WRP1B_END=0x0
set boot_lock=BOOT_LOCK=1
:: =============================================== Configure Option Bytes ====================================================================
set "action=Remove Protection and Erase All"
echo %action%
%stm32programmercli% %connect_reset% %remove_protect_init% %erase_all%
IF !errorlevel! NEQ 0 goto :error
:: ==================================================== Download images ====================================================================
echo "Application images programming in primary slots"

set "action=Write OEMiROT_Boot"
echo %action%
%stm32programmercli% %connect_no_reset% -d %cube_fw_path%\Projects\NUCLEO-U083RC\Applications\ROT\OEMiROT_Boot\Binary\OEMiROT_Boot.bin 0x8000000 -v
IF !errorlevel! NEQ 0 goto :error
echo "OEMiROT_Boot Written"

set "action=Write OEMiROT_Appli"
echo %action%
%stm32programmercli% %connect_no_reset% -d ../../%oemirot_boot_path_project%/Binary/%code_image% %appliaddress% -v
IF !errorlevel! NEQ 0 goto :error
echo "OEMiROT_Appli Written"

set "action=Write OEMiROT_Data"
echo %action%
%stm32programmercli% %connect_no_reset% -d %cube_fw_path%\Projects\NUCLEO-U083RC\ROT_Provisioning\OEMiROT\Binary\data_enc_sign.bin %dataaddress% -v
IF !errorlevel! NEQ 0 goto :error
echo "OEMiROT_Data Written"

set "action=Write OEMiROT_Loader"
echo %action%
%stm32programmercli% %connect_no_reset% -d %cube_fw_path%\Projects\NUCLEO-U083RC\Applications\ROT\OEMiROT_Loader\Binary\OEMiROT_Loader.bin %loaderaddress% -v
IF !errorlevel! NEQ 0 goto :error
echo "OEMiROT_Boot Written"

:: ======================================================= Extra board protections =========================================================
echo "Configure Option Bytes: Write Protection, Hide Protection and Boot Lock"
%stm32programmercli% %connect_no_reset% -ob %boot_write_protect% %loader_write_protect% %hide_protect% %boot_lock%
IF !errorlevel! NEQ 0 goto :error

echo Programming success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo      Error when trying to "%action%" >CON
echo      Programming aborted >CON
IF [%1] NEQ [AUTO] cmd /k
exit 1

