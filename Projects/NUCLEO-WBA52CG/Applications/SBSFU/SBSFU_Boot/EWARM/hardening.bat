set sec1_end=
set wrpa_start=
set wrpa_end=
set wrpb_start=
set wrpb_end=
set hdp_end=
set nsbootadd=
echo hardening script started
set stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
set connect=-c port=SWD mode=UR ap=1 --hardRst
set wrp_sbsfu=WRPA_PSTRT=%wrpa_start% WRPA_PEND=%wrpa_end%
set wrp_loader=WRPB_PSTRT=%wrpb_start% WRPB_PEND=%wrpb_end%
set nsboot_add_set=NSBOOTADD0=%nsbootadd% NSBOOTADD1=%nsbootadd%
set write_protect_secure=-ob %wrp_sbsfu% %wrp_loader% SECWM1_PEND=%sec1_end% HDP1_PEND=%hdp_end% HDP1EN=1 %nsboot_add_set% BOOT_LOCK=1
%stm32programmercli% %connect% %write_protect_secure%
IF %errorlevel% NEQ 0 goto :error
echo hardening script done, press key
IF [%1] NEQ [AUTO] pause
exit 0

:error
echo hardening script failed, press key
IF [%1] NEQ [AUTO] pause
exit 1
