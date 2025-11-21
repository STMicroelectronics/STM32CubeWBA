#!/bin/bash
if [ $# -ge 1 ]; then script_mode=$1; else script_mode=MANUAL; fi
param2=$2
rdp_lev=$param2
source ../env.sh
# Get config updated by OEMuROT_Boot
source img_config.sh

# Error when external script is executed.
error()
{
  echo "        Error when trying to "$action #> "ob_flash_programming.log"
  echo "        Programming aborted" #>> "ob_flash_programming.log"
  if [ "$script_mode" != "AUTO" ]; then $SHELL; fi
  return 1
}

bootpath=OEMiRoT_OEMuRoT

# Data updated with the postbuild of OEMuROT-Boot
sec1_start=0x0
sec1_end=0x1B
wrp_a_bank1_start=0x2
wrp_a_bank1_end=0xB
wrp_b_bank1_start=0x0
wrp_b_bank1_end=0x0
hdp_end=0x16
secbootadd0=0x1800C0
flashsectnbr=0x7F

slot0=0xC030000
slot1=0xC038000
slot2=0xC06A000
slot3=0xC072000
slot4=0x0
slot5=0x0
slot6=0x0
slot7=0x0

keyaddress=0xC01C000
bootaddress=0xC006000
oemirot_oemurot_slot0=0xC01E000
select_wba_target=0x65
is_wba_board_dk=1
overwrite=1

s_code_image=$oemurot_appli_secure
ns_code_image=$oemurot_appli_non_secure
one_code_image=$oemurot_appli_assembly_sign
s_data_image=s_data_init_sign.bin
ns_data_image=ns_data_init_sign.bin

# STM32CubeProgammer connection
connect="-c port=SWD mode=UR"
connect_no_reset="-c port=SWD mode=Hotplug"

# Get config updated by OEMiROT_Boot
appli_dir="../../$oemirot_appli_path_project"
boot_dir="../../$oemirot_boot_path_project"

# =============================================== Remove protections and initialize Option Bytes ==========================================
remove_protect_init="SRAM2_RST=0"
# lock write protection when rdp level >= 1
wrp_protect=
if [ "$select_wba_target" == "0x55" ]; then
  remove_bank1_protect="-ob SECWM_PSTRT=$flashsectnbr SECWM_PEND=0 WRPA_PSTRT=$flashsectnbr WRPA_PEND=0 WRPB_PSTRT=$flashsectnbr WRPB_PEND=0 $remove_protect_init"
  remove_hdp_protection="-ob HDP_PEND=0 HDPEN=0"
  if [ "$rdp_lev" -ge 1 ]; then
    wrp_protect="UNLOCK_A=0 UNLOCK_B=0"
  fi
elif [ "$select_wba_target" == "0x65" ]; then
  remove_bank1_protect="-ob SECWM1_PSTRT=$flashsectnbr SECWM1_PEND=0 WRP1A_PSTRT=$flashsectnbr WRP1A_PEND=0 WRP1B_PSTRT=$flashsectnbr WRP1B_PEND=0 $remove_protect_init"
  remove_bank2_protect="-ob SECWM2_PSTRT=$flashsectnbr SECWM2_PEND=0 WRP2A_PSTRT=$flashsectnbr WRP2A_PEND=0 WRP2B_PSTRT=$flashsectnbr WRP2B_PEND=0"
  remove_hdp_protection="-ob HDP1_PEND=0 HDP1EN=0 HDP2_PEND=0 HDP2EN=0"
  if [ "$rdp_lev" -ge 1 ]; then
	wrp_protect="UNLOCK_1A=0 UNLOCK_1B=0 UNLOCK_2A=0 UNLOCK_2B=0"
  fi
else
  echo "No target selected"
  error;
fi
erase_all="-e all"
remove_boot_lock="-ob BOOT_LOCK=0"

# =============================================================== Hardening ===============================================================
boot_address="-ob SECBOOTADD0=$secbootadd0 NSBOOTADD0=$secbootadd0 NSBOOTADD1=$secbootadd0"
boot_lock="BOOT_LOCK=1"
if [ "$select_wba_target" == "0x55" ]; then
  write_protect="WRPA_PSTRT=$wrp_a_bank1_start WRPA_PEND=$wrp_a_bank1_end"
  hide_protect="HDP_PEND=$hdp_end HDPEN=1"
  sec_water_mark="SECWM_PSTRT=$sec1_start SECWM_PEND=$sec1_end"
else
  write_protect="WRP1A_PSTRT=$wrp_a_bank1_start WRP1A_PEND=$wrp_a_bank1_end"
  hide_protect="HDP1_PEND=$hdp_end HDP1EN=1"
  sec_water_mark="SECWM1_PSTRT=$sec1_start SECWM1_PEND=$sec1_end"
fi

# =============================================== Configure Option Bytes ==================================================================
action="Set TZEN = 1"
echo "$action"
# Trust zone enabled is mandatory in order to execute OEM-uRoT
"$stm32programmercli" $connect_no_reset -ob TZEN=1
if [ $? -ne 0 ]; then error; return 1; fi

action="Remove hdp protection"
echo "$action"
"$stm32programmercli" $connect_no_reset $remove_hdp_protection
if [ $? -ne 0 ]; then error; return 1; fi

action="Remove bank1 protection : $remove_bank1_protect"
echo "$action"
"$stm32programmercli" $connect $remove_bank1_protect
if [ $? -ne 0 ]; then error; return 1; fi

if [ "$select_wba_target" == "0x65" ]; then
  action="Remove bank2 protection : $remove_bank2_protect"
  echo "$action"
  "$stm32programmercli" $connect $remove_bank2_protect
  if [ $? -ne 0 ]; then error; return 1; fi
fi

action="Erase all"
echo "$action"
"$stm32programmercli" $connect_no_reset $erase_all
if [ $? -ne 0 ]; then error; return 1; fi

action="Remove Boot lock"
echo "$action"
"$stm32programmercli" $connect_no_reset $remove_boot_lock
if [ $? -ne 0 ]; then error; return 1; fi

set "action=Set boot address @$secbootadd0"
echo "$action"
"$stm32programmercli" $connect_no_reset $boot_address
if [ $? -ne 0 ]; then error; return 1; fi

# =============================================== Download images =========================================================================
echo "Application images programming in download slots"

if [ "$app_image_number" == "2" ]; then
    action="Write Appli Secure"
    echo "$action $appli_dir/Binary/$s_code_image"
    "$stm32programmercli" $connect_no_reset -d "$appli_dir/Binary/$s_code_image" $slot0 -v --skipErase
    if [ $? -ne 0 ]; then error; return 1; fi
    echo "TZ Appli Secure Written"
    action="Write Appli NonSecure"
    echo "$action"
    "$stm32programmercli" $connect_no_reset -d "$appli_dir/Binary/$ns_code_image" $slot1 -v --skipErase
    if [ $? -ne 0 ]; then error; return 1; fi
    echo "TZ Appli NonSecure Written"
fi

if [ "$app_image_number" == "1" ]; then
    if [ "$app_full_secure" == "1" ]; then
        action="Write Appli Full Secure"
        echo "$action $appli_dir/Binary/$s_code_image"
        "$stm32programmercli" $connect_no_reset -d "$appli_dir/Binary/$s_code_image" $slot0 -v --skipErase
        if [ $? -ne 0 ]; then error; return 1; fi
        echo "Appli Full Secure Written"
    else
        action="Write One image Appli"
        echo "$action"
        "$stm32programmercli" $connect_no_reset -d "$appli_dir/Binary/$one_code_image" $slot0 -v --skipErase
        if [ $? -ne 0 ]; then error; return 1; fi

        echo "TZ Appli Written"
    fi
fi

if [ "$s_data_image_number" == "1" ]; then
    action="Write Secure Data"
    echo "$action"
    if [ ! -f "$rot_provisioning_path/$bootpath/Binary/$s_data_image" ]; then
        echo "Error: $s_data_image does not exist! use TPC to generate it"
        error
    fi

    "$stm32programmercli" $connect_no_reset -d "$rot_provisioning_path/$bootpath/Binary/$s_data_image" $slot4 -v --skipErase
    if [ $? -ne 0 ]; then error; return 1; fi
fi

if [ "$ns_data_image_number" == "1" ]; then
    action="Write non Secure Data"
    echo "$action"
    if [ ! -f "$rot_provisioning_path/$bootpath/Binary/$ns_data_image" ]; then
        echo "Error: $ns_data_image does not exist! use TPC to generate it"
        error
    fi

    "$stm32programmercli" $connect_no_reset -d "$rot_provisioning_path/$bootpath/Binary/$ns_data_image" $slot5 -v --skipErase
    if [ $? -ne 0 ]; then error; return 1; fi
fi

action="Write Binary Keys of first boot Stage"
echo "$action"
"$stm32programmercli" "$connect_no_reset" -d "$rot_provisioning_path/OEMiRoT_OEMuRoT/Binary/OEMiRoT_Keys.bin" 0xC004000 -v --skipErase
if [ $? -ne 0 ]; then error; return 1; fi
echo "Binary Keys Written"

action="Write the first boot stage OEMiROT_Boot"
echo "$action"
# write the first boot stage specifically for the board
if [ "$is_wba_board_dk" == "0" ] && [ "$select_wba_target" == "0x55" ]; then
  if [ "$overwrite" == "1" ]; then
    first_boot_stage_binary=OEMiROT_Boot_NUCLEO_WBA55CG_OVERWRITE.bin
  else
    first_boot_stage_binary=OEMiROT_Boot_NUCLEO_WBA55CG_SWAP.bin
  fi
  if [ $? -ne 0 ]; then error; return 1; fi

elif [ "$is_wba_board_dk" == "1" ] && [ "$select_wba_target" == "0x55" ]; then
  if [ "$overwrite" == "1" ]; then
    first_boot_stage_binary=OEMiROT_Boot_STM32WBA55G_DK1_OVERWRITE.bin
  else
    first_boot_stage_binary=OEMiROT_Boot_STM32WBA55G_DK1_SWAP.bin
  fi
  if [ $? -ne 0 ]; then error; return 1; fi

elif [ "$is_wba_board_dk" == "0" ] && [ "$select_wba_target" == "0x65" ]; then
  if [ "$overwrite" == "1" ]; then
    first_boot_stage_binary=OEMiROT_Boot_NUCLEO_WBA65RI_OVERWRITE.bin
  else
    first_boot_stage_binary=OEMiROT_Boot_NUCLEO_WBA65RI_SWAP.bin
  fi
  if [ $? -ne 0 ]; then error; return 1; fi

elif [ "$is_wba_board_dk" == "1" ] && [ "$select_wba_target" == "0x65" ]; then
  if [ "$external_flash" == "1" ]; then
    if [ "$overwrite" == "1" ]; then
      first_boot_stage_binary=OEMiROT_Boot_ExtFlsh_STM32WBA65I_DK1_OVERWRITE.bin
    else
      first_boot_stage_binary=OEMiROT_Boot_ExtFlsh_STM32WBA65I_DK1_SWAP.bin
    fi
  else
    if [ "$overwrite" == "1" ]; then
      first_boot_stage_binary=OEMiROT_Boot_STM32WBA65I_DK1_OVERWRITE.bin
    else
      first_boot_stage_binary=OEMiROT_Boot_STM32WBA65I_DK1_SWAP.bin
  fi
  if [ $? -ne 0 ]; then error; return 1; fi

else
  echo "No board selected"
  error;
fi

"$stm32programmercli" "$connect_no_reset" -d "$rot_provisioning_path/OEMiRoT_OEMuRoT/Binary/$first_boot_stage_binary" 0xC006000 -v --skipErase
if [ $? -ne 0 ]; then error; return 1; fi
echo "OEMiROT_Boot Written"

action="Write Binary Keys of the second boot stage"
echo "$action"
"$stm32programmercli" "$connect_no_reset" -d "$rot_provisioning_path/OEMiRoT_OEMuRoT/Binary/OEMuRoT_Keys.bin" $keyaddress -v --skipErase
if [ $? -ne 0 ]; then error; return 1; fi
echo "Binary Keys Written"

action="Write Write the second boot stage OEMuROT_Boot"
echo "$action"
"$stm32programmercli" "$connect_no_reset" -d "$boot_dir/Binary/OEMuROT_Boot_init_sign.bin" $oemirot_oemurot_slot0 -v --skipErase
if [ $? -ne 0 ]; then error; return 1; fi
echo "OEMuROT_Boot Written"

# ======================================================= Extra board protections =========================================================
action="Configure Option Bytes"
echo "$action"
echo "Configure Secure option Bytes: Write Protection, Hide Protection, boot lock and Write protection"
"$stm32programmercli" $connect_no_reset -ob $sec_water_mark $write_protect $hide_protect $boot_lock $wrp_protect
if [ $? -ne 0 ]; then error; return 1; fi

echo "Programming success"
if [ "$script_mode" != "AUTO" ]; then $SHELL;  fi

return
