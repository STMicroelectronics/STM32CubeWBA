echo regression script started
set stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
set secbootadd0=
set flashsectnbr=
set connect=-c port=SWD mode=UR ap=1 --hardRst
set connect_no_reset=-c port=SWD ap=1 mode=HotPlug
set rdp_0=-ob RDP=0xAA TZEN=1 UNLOCK_A=1 UNLOCK_B=1
set remove_bank1_protect=-ob SECWM_PSTRT=%flashsectnbr% SECWM_PEND=0 WRPA_PSTRT=%flashsectnbr% WRPA_PEND=0 WRPB_PSTRT=%flashsectnbr% WRPB_PEND=0
set erase_all=-e all
set remove_hdp_protection=-ob HDP_PEND=0 HDPEN=0
set default_ob1=-ob SRAM2_RST=0 NSBOOTADD0=0x100000 NSBOOTADD1=0x17f200 BOOT_LOCK=0 SECBOOTADD0=%secbootadd0% nSWBOOT0=1 SECWM_PSTRT=0 SECWM_PEND=%flashsectnbr%
set oem_passwd2=0xFACEB00C 0xDEADBABE
set oem_lock2=-lockRDP2 %oem_passwd2%
echo Regression to RDP 0, enable tz
%stm32programmercli% %connect_no_reset% %rdp_0%
IF %errorlevel% NEQ 0 %stm32programmercli% %connect% %rdp_0%
IF %errorlevel% NEQ 0 goto :error
echo Provision default OEM2 key
%stm32programmercli% %connect% %oem_lock2%
IF %errorlevel% NEQ 0 goto :error
echo Remove bank1 protection
%stm32programmercli% %connect% %remove_bank1_protect%
IF %errorlevel% NEQ 0 goto :error
echo Remove bank2 protection and erase all
%stm32programmercli% %connect_no_reset% %remove_bank2_protect% %erase_all%
IF %errorlevel% NEQ 0 goto :error
echo Remove hdp protection
%stm32programmercli% %connect_no_reset% %remove_hdp_protection%
IF %errorlevel% NEQ 0 goto :error
echo Set default OB 1 (dual bank, swap bank, sram2 reset, secure entry point, bank 1 full secure)
%stm32programmercli% %connect_no_reset% %default_ob1%
IF %errorlevel% NEQ 0 goto :error
echo Set default OB 2 (bank 2 full secure)
%stm32programmercli% %connect_no_reset% %default_ob2%
IF %errorlevel% NEQ 0 goto :error
echo regression script done, press key
IF [%1] NEQ [AUTO] pause
exit 0

:error
echo regression script failed, press key
IF [%1] NEQ [AUTO] pause
exit 1
