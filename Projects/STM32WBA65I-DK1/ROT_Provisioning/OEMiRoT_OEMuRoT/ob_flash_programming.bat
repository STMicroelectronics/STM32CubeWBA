call ../env.bat
:: Get config updated by OEMuROT_Boot
call ./img_config.bat

set param2=%2
set rdp_lev=%param2%
set rot_provisioning_path=%rot_provisioning_path:"=%
:: Enable delayed expansion
setlocal EnableDelayedExpansion

set bootpath=OEMiRoT_OEMuRoT

:: Data updated with the postbuild of OEMuROT-Boot
set sec1_start=0x0
set sec1_end=0x1B
set wrp_a_bank1_start=0x2
set wrp_a_bank1_end=0xB
set wrp_b_bank1_start=0x0
set wrp_b_bank1_end=0x0
set hdp_end=0x16
set secbootadd0=0x1800C0
set flashsectnbr=0x7F

set slot0=0xC030000
set slot1=0xC038000
set slot2=0xC06A000
set slot3=0xC072000
set slot4=0x0
set slot5=0x0
set slot6=0x0
set slot7=0x0

set keyaddress=0xC01C000
set bootaddress=0xC006000
set oemirot_oemurot_slot0=0xC01E000
set select_wba_target=0x65
set is_wba_board_dk=1
set overwrite=1

set s_code_image=%oemurot_appli_secure%
set ns_code_image=%oemurot_appli_non_secure%
set one_code_image=%oemurot_appli_assembly_sign%
set s_data_image=s_data_init_sign.bin
set ns_data_image=ns_data_init_sign.bin

:: STM32CubeProgammer connection
set connect=-c port=SWD mode=UR
set connect_no_reset=-c port=SWD mode=Hotplug

:: Get config updated by OEMiROT_Boot
set appli_dir=..\..\%oemirot_appli_path_project%
set boot_dir=..\..\%oemirot_boot_path_project%

:: =============================================== Remove protections and initialize Option Bytes ==========================================
set remove_protect_init=SRAM2_RST=0
:: lock write protection when rdp level >= 1
set wrp_protect=
IF "%select_wba_target%" == "0x65" (goto ob_stm32wba65xx)
set remove_bank1_protect=-ob SECWM_PSTRT=%flashsectnbr% SECWM_PEND=0 WRPA_PSTRT=%flashsectnbr% WRPA_PEND=0 WRPB_PSTRT=%flashsectnbr% WRPB_PEND=0 %remove_protect_init%
set remove_hdp_protection=-ob HDP_PEND=0 HDPEN=0
if %rdp_lev% GEQ 1 (
echo "Configure wrp_protect"
	set wrp_protect=UNLOCK_A=0 UNLOCK_B=0
)
goto ob_stm32wba55xx
:ob_stm32wba65xx
set remove_bank1_protect=-ob SECWM1_PSTRT=%flashsectnbr% SECWM1_PEND=0 WRP1A_PSTRT=%flashsectnbr% WRP1A_PEND=0 WRP1B_PSTRT=%flashsectnbr% WRP1B_PEND=0 %remove_protect_init%
set remove_bank2_protect=-ob SECWM2_PSTRT=%flashsectnbr% SECWM2_PEND=0 WRP2A_PSTRT=%flashsectnbr% WRP2A_PEND=0 WRP2B_PSTRT=%flashsectnbr% WRP2B_PEND=0
set remove_hdp_protection=-ob HDP1_PEND=0 HDP1EN=0 HDP2_PEND=0 HDP2EN=0
if %rdp_lev% GEQ 1 (
	set wrp_protect=UNLOCK_1A=0 UNLOCK_1B=0 UNLOCK_2A=0 UNLOCK_2B=0
)
:ob_stm32wba55xx
:: Check if the target is selected
IF "%remove_hdp_protection%"=="" (
echo No target selected.
goto :error
)
set erase_all=-e all
set remove_boot_lock=-ob BOOT_LOCK=0

:: =============================================================== Hardening ===============================================================
set boot_address=-ob SECBOOTADD0=%secbootadd0% NSBOOTADD0=%secbootadd0% NSBOOTADD1=%secbootadd0%
set boot_lock=BOOT_LOCK=1
IF "%select_wba_target%" == "0x65" (goto ob_stm32wba65xx)
set write_protect=WRPA_PSTRT=%wrp_a_bank1_start% WRPA_PEND=%wrp_a_bank1_end%
set hide_protect=HDP_PEND=%hdp_end% HDPEN=1
set sec_water_mark=SECWM_PSTRT=%sec1_start% SECWM_PEND=%sec1_end%
goto ob_stm32wba55xx
:ob_stm32wba65xx
set write_protect=WRP1A_PSTRT=%wrp_a_bank1_start% WRP1A_PEND=%wrp_a_bank1_end%
set hide_protect=HDP1_PEND=%hdp_end% HDP1EN=1
set sec_water_mark=SECWM1_PSTRT=%sec1_start% SECWM1_PEND=%sec1_end%
:ob_stm32wba55xx

:: =============================================== Configure Option Bytes ==================================================================
set "action=Set TZEN = 1"
echo %action%
:: Trust zone enabled is mandatory in order to execute OEM-iRoT
%stm32programmercli% %connect_no_reset% -ob TZEN=1
IF !errorlevel! NEQ 0 goto :error

set "action=Remove hdp protection"
echo %action%
%stm32programmercli% %connect_no_reset% %remove_hdp_protection%
IF !errorlevel! NEQ 0 goto :error

set "action=Remove bank1 protection : %remove_bank1_protect%"
echo %action%
%stm32programmercli% %connect% %remove_bank1_protect%
IF !errorlevel! NEQ 0 goto :error

IF "%select_wba_target%" == "0x55" (goto ob_stm32wba55xx)
set "action=Remove bank2 protection : %remove_bank2_protect%"
echo %action%
%stm32programmercli% %connect% %remove_bank2_protect%
IF !errorlevel! NEQ 0 goto :error

:ob_stm32wba55xx
set "action=Erase all"
echo %action%
%stm32programmercli% %connect_no_reset% %erase_all%
IF !errorlevel! NEQ 0 goto :error

set "action=Remove Boot lock"
echo %action%
%stm32programmercli% %connect_no_reset% %remove_boot_lock%
IF !errorlevel! NEQ 0 goto :error

set "action=Set boot address @%secbootadd0%"
echo %action%
%stm32programmercli% %connect_no_reset% %boot_address%
IF !errorlevel! NEQ 0 goto :error

:: =============================================== Download images =========================================================================
echo "Application images programming in download slots"

IF  "%app_image_number%" == "2" (
set "action=Write Appli Secure"
echo %action%
%stm32programmercli% %connect_no_reset% -d %appli_dir%\Binary\%s_code_image% %slot0% -v --skipErase
IF !errorlevel! NEQ 0 goto :error
echo "TZ Appli Secure Written"
set "action=Write Appli NonSecure"
echo %action%
%stm32programmercli% %connect_no_reset% -d %appli_dir%\Binary\%ns_code_image% %slot1% -v --skipErase
IF !errorlevel! NEQ 0 goto :error
echo "TZ Appli NonSecure Written"
)

IF  "%app_image_number%" == "1" (
IF  "%app_full_secure%" == "1" (
set "action=Write Appli Full Secure"
echo %action%
%stm32programmercli% %connect_no_reset% -d %appli_dir%\Binary\%s_code_image% %slot0% -v --skipErase
IF !errorlevel! NEQ 0 goto :error
echo "Appli Full Secure Written"
) else (
set "action=Write One image Appli"
echo %action%
%stm32programmercli% %connect_no_reset% -d %appli_dir%\Binary\%one_code_image% %slot0% -v --skipErase
IF !errorlevel! NEQ 0 goto :error

echo "TZ Appli Written"
)
)

IF  "%s_data_image_number%" == "1" (
set "action=Write Secure Data"
echo %action%
IF not exist %rot_provisioning_path%\%bootpath%\Binary\%s_data_image% (
@echo [31mError: %s_data_image% does not exist! use TPC to generate it[0m
goto :error
)

%stm32programmercli% %connect_no_reset% -d %rot_provisioning_path%\%bootpath%\Binary\%s_data_image% %slot4% -v --skipErase
IF !errorlevel! NEQ 0 goto :error
)

IF  "%ns_data_image_number%" == "1" (
set "action=Write non Secure Data"
echo %action%
IF not exist %rot_provisioning_path%\%bootpath%\Binary\%ns_data_image% (
@echo [31mError: %ns_data_image% does not exist! use TPC to generate it[0m
goto :error
)

%stm32programmercli% %connect_no_reset% -d %rot_provisioning_path%\%bootpath%\Binary\%ns_data_image% %slot5% -v --skipErase
IF !errorlevel! NEQ 0 goto :error
)

set "action=Write Binary Keys of first boot Stage"
echo %action%
%stm32programmercli% %connect_no_reset% -d %rot_provisioning_path%\%bootpath%\Binary\OEMiRoT_Keys.bin 0xC004000 -v --skipErase
IF !errorlevel! NEQ 0 goto :error
echo "Binary Keys Written"

set "action=Write the first boot stage OEMiROT_Boot"
echo %action%
:: write the first boot stage specifically for the board
IF "%is_wba_board_dk%"  == "0" (
IF "%select_wba_target%" == "0x55" (
IF "%overwrite%" == "1" (
set first_boot_stage_binary=OEMiROT_Boot_NUCLEO_WBA55CG_OVERWRITE.bin
) else (
set first_boot_stage_binary=OEMiROT_Boot_NUCLEO_WBA55CG_SWAP.bin
)
)
)

IF "%is_wba_board_dk%"  == "1" (
IF "%select_wba_target%" == "0x55" (
IF "%overwrite%" == "1" (
set first_boot_stage_binary=OEMiROT_Boot_STM32WBA55G_DK1_OVERWRITE.bin
) else (
set first_boot_stage_binary=OEMiROT_Boot_STM32WBA55G_DK1_SWAP.bin
)
)
)

IF "%is_wba_board_dk%"  == "0" (
IF "%select_wba_target%" == "0x65" (
IF "%overwrite%" == "1" (
set first_boot_stage_binary=OEMiROT_Boot_NUCLEO_WBA65RI_OVERWRITE.bin
) else (
set first_boot_stage_binary=OEMiROT_Boot_NUCLEO_WBA65RI_SWAP.bin
)
)
)

IF "%is_wba_board_dk%"  == "1" (
IF "%select_wba_target%" == "0x65" (
IF "%external_flash%" == "1" (
IF "%overwrite%" == "1" (
set first_boot_stage_binary=OEMiROT_Boot_ExtFlsh_STM32WBA65I_DK1_OVERWRITE.bin
) else (
set first_boot_stage_binary=OEMiROT_Boot_ExtFlsh_STM32WBA65I_DK1_SWAP.bin
)
) else (
IF "%overwrite%" == "1" (
set first_boot_stage_binary=OEMiROT_Boot_STM32WBA65I_DK1_OVERWRITE.bin
) else (
set first_boot_stage_binary=OEMiROT_Boot_STM32WBA65I_DK1_SWAP.bin
)
)
)
)

%stm32programmercli% %connect_no_reset% -d %rot_provisioning_path%\OEMiRoT_OEMuRoT\Binary\%first_boot_stage_binary% 0xC006000 -v --skipErase
IF !errorlevel! NEQ 0 goto :error
echo "OEMiROT_Boot Written"

set "action=Write Binary Keys of the second boot stage"
echo %action%
%stm32programmercli% %connect_no_reset% -d %rot_provisioning_path%\OEMiRoT_OEMuRoT\Binary\OEMuRoT_Keys.bin %keyaddress% -v --skipErase
IF !errorlevel! NEQ 0 goto :error
echo "Binary Keys Written"

set "action=Write Write the second boot stage OEMuROT_Boot"
echo %action%
%stm32programmercli% %connect_no_reset% -d %boot_dir%\Binary\OEMuROT_Boot_init_sign.bin %oemirot_oemurot_slot0% -v --skipErase
IF !errorlevel! NEQ 0 goto :error
echo "OEMuROT_Boot Written"

:: ======================================================= Extra board protections =========================================================
set "action=Configure Option Bytes"
echo %action%
echo "Configure Secure option Bytes: Write Protection, Hide Protection, boot lock and Write protection"
%stm32programmercli% %connect_no_reset% -ob %sec_water_mark% %write_protect% %hide_protect% %boot_lock% %wrp_protect%
IF !errorlevel! NEQ 0 goto :error

echo Programming success
IF [%1] NEQ [AUTO] cmd /k
exit 0

:error
echo      Error when trying to "%action%" >CON
echo      Programming aborted >CON
echo.
IF [%1] NEQ [AUTO] cmd /k
exit 1
