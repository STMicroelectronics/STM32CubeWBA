#!/bin/bash -
echo "regression script started"
PATH="/C/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/":$PATH
stm32programmercli="STM32_Programmer_CLI"
secbootadd0=
flashsectnbr=
connect="-c port=SWD mode=UR ap=1 --hardRst"
connect_no_reset="-c port=SWD ap=1 mode=HotPlug"
rdp_0="-ob RDP=0xAA TZEN=1 UNLOCK_A=1 UNLOCK_B=1"
remove_bank1_protect="-ob SECWM1_PSTRT="$flashsectnbr" SECWM1_PEND=0 WRPA_PSTRT="$flashsectnbr" WRPA_PEND=0 WRPB_PSTRT="$flashsectnbr" WRPB_PEND=0"
erase_all="-e all"
remove_hdp_protection="-ob HDP1_PEND=0 HDP1EN=0"
default_ob1="-ob SRAM2_RST=0 NSBOOTADD0=0x100000 NSBOOTADD1=0x17f200 BOOT_LOCK=0 SECBOOTADD0="$secbootadd0" nSWBOOT0=1 SECWM1_PSTRT=0 SECWM1_PEND="$flashsectnbr""
oem_passwd2="0xFACEB00C 0xDEADBABE"
oem_lock2="-lockRDP2 "$oem_passwd2""
echo "Regression to RDP 0, enable tz"
$stm32programmercli $connect_no_reset $rdp_0
ret=$?
if [ $ret != 0 ]; then
  $stm32programmercli $connect $rdp_0
  ret=$?
  if [ $ret != 0 ]; then
    if [ "$1" != "AUTO" ]; then read -p "regression script failed, press a key" -n1 -s; fi
    exit 1
  fi
fi
echo "Provision default OEM2 key"
$stm32programmercli $connect $oem_passwd2
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "regression script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "Remove bank1 protection"
$stm32programmercli $connect $remove_bank1_protect
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "regression script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "Erase all"
$stm32programmercli $connect_no_reset $erase_all
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "regression script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "Remove hdp protection"
$stm32programmercli $connect_no_reset $remove_hdp_protection
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "regression script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "Set default OB 1 (dual bank, swap bank, sram2 reset, secure entry point, bank 1 full secure)"
$stm32programmercli $connect_no_reset $default_ob1
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "regression script failed, press a key" -n1 -s; fi
  exit 1
fi
echo "Set default OB 2 (bank 2 full secure)"
$stm32programmercli $connect_no_reset $default_ob2
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "regression script failed, press a key" -n1 -s; fi
  exit 1
fi
if [ "$1" != "AUTO" ]; then read -p "regression script Done, press a key" -n1 -s; fi
exit 0
