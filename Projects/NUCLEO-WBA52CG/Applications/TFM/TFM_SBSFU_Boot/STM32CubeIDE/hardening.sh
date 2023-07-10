#!/bin/bash -
sec1_end=
wrp_start=
wrp_end=
hdp_end=
wrp_bank2_start=
wrp_bank2_end=
nsbootadd=
echo "hardening script started"
PATH="/C/Program Files/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/":$PATH
stm32programmercli="STM32_Programmer_CLI"
connect="-c port=SWD mode=UR ap=1 --hardRst"
wrp_sbsfu="WRPA_PSTRT="$wrp_start" WRPA_PEND="$wrp_end
wrp_loader="WRPB_PSTRT="$wrp_bank2_start" WRPB_PEND="$wrp_bank2_end
nsboot_add_set="NSBOOTADD0="$nsbootadd" NSBOOTADD1="$nsbootadd
write_protect_secure="-ob "$wrp_sbsfu" "$wrp_loader" SECWM_PEND="$sec1_end" HDP_PEND="$hdp_end" HDPEN=1 "$nsboot_add_set" BOOT_LOCK=1"
$stm32programmercli $connect $write_protect_secure
ret=$?
if [ $ret != 0 ]; then
  if [ "$1" != "AUTO" ]; then read -p "hardening script failed, press a key" -n1 -s; fi
  exit 1
fi
if [ "$1" != "AUTO" ]; then read -p "hardening script Done, press a key" -n1 -s; fi
exit 0