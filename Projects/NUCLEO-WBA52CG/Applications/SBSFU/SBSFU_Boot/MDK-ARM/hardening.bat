set sec1_end=
set wrp_start=
set wrp_end=
set hdp_end=
set wrp_bank2_start=
set wrp_bank2_end=
set nsbootadd=
echo hardening script started
call ..\..\env.bat
set connect=-c port=SWD mode=UR ap=1
set wrp_sbsfu=WRPA_PSTRT=%wrp_start% WRPA_PEND=%wrp_end%
set wrp_loader=WRPB_PSTRT=%wrp_bank2_start% WRPB_PEND=%wrp_bank2_end%
set nsboot_add_set=NSBOOTADD0=%nsbootadd% NSBOOTADD1=%nsbootadd%
set write_protect_secure=-ob %wrp_sbsfu% %wrp_loader% SECWM_PEND=%sec1_end% HDP_PEND=%hdp_end% HDPEN=1 %nsboot_add_set% BOOT_LOCK=1
%stm32programmercli% %connect% %write_protect_secure%
IF %errorlevel% NEQ 0 goto :error
echo hardening script done, press key
IF [%1] NEQ [AUTO] pause
exit 0

:error
echo hardening script failed, press key
IF [%1] NEQ [AUTO] pause
exit 1
