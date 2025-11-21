set stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
set connect_no_reset=-c port=SWD ap=1 mode=Hotplug
set connect_reset=-c port=SWD ap=1 mode=UR

:: =============================================== Remove protections and erase the user flash ===============================================

set remove_protect=-ob SECWM1_STRT=1 SECWM1_END=0 WRPSGn1=0xffffffff WRPSGn2=0xffffffff SECWM2_STRT=1 SECWM2_END=0 HDP1_STRT=1 HDP1_END=0 HDP2_STRT=1 HDP2_END=0 SECBOOT_LOCK=0xC3
set erase_all=-e all

:: =============================================== Configure Option Bytes ====================================================================

echo "Set TZEN = 1"
:: Trust zone enabled is mandatory inorder to execute ST-iRoT
%stm32programmercli% %connect_no_reset% -ob TZEN=0xB4
IF %errorlevel% NEQ 0 goto :error

echo "Remove Protection and erase All"
%stm32programmercli% %connect_reset% %remove_protect% %erase_all%

echo "Set SRAM 2 configuration"
:: Recommanded configuration for secure boot is :
::   - SRAM2 erased in case of reset ==> SRAM2_RST=0
::   - SRAM2 ECC actovated. Hack tentativ detectection enabled ==> SRAM2_ECC=0 
%stm32programmercli% %connect_no_reset% -ob SRAM2_RST=1 SRAM2_ECC=1
IF %errorlevel% NEQ 0 goto :error 

echo "Define secure area through watermarks"
:: This configuration depends on user mapping but the watermarks should cover at least the secure area part of the firmware execution slot.
:: The secure area can also be larger in order to include additionnal sectors. For example the secure firmware will have to manage user data.
%stm32programmercli% %connect_no_reset% -ob SECWM1_STRT=0x0 SECWM1_END=0xf
IF %errorlevel% NEQ 0 goto :error
%stm32programmercli% %connect_no_reset% -ob SECWM2_STRT=0x7f SECWM2_END=0x0
IF %errorlevel% NEQ 0 goto :error

:: ==================================================== Download images ====================================================================

echo "Application images programming in download slots"
%stm32programmercli% %connect_no_reset% -d %~dp0..\..\ROT_Appli\binary\appli_enc_sign.hex
IF %errorlevel% NEQ 0 goto :error

%stm32programmercli% %connect_no_reset% -d %~dp0.\data_enc_sign.hex
IF %errorlevel% NEQ 0 goto :error

echo "Programming success"
cmd /k
exit 0

:error
echo "Programming aborted"
cmd /k
exit 1
