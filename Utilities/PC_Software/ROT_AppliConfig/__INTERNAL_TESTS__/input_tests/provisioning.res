#!/bin/bash

# Getting the CubeProgammer_cli path
source ../env.sh

SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`
if [ $# -ge 1 ]; then mode=$1; else mode=MANUAL; fi

data_hex_val="0x10000"
subregion_val="0x0"
rdp_value="0xAA"
rdp_str="OB_RDP_LEVEL_0"
action=""

final_execution() {
    echo "======"
    echo "====== The board is correctly configured."
    echo "======"
    if [ "$mode" != "AUTO" ]; then $SHELL; fi
       exit 0
}

step_error() {
    echo ""
    echo "======"
    echo "====== Error while executing \"$action\"."
    echo "====== See $current_log_file for details. Then try again."
    echo "======"
    if [ "$mode" != "AUTO" ]; then $SHELL; fi
       exit 1
}

define_rdp_level() {
    action="Define final RDP value"
    echo "* $action"
    read -p "      ${USERREG} [ 0 | 1 | 2 ]: " RDP_level
	RDP_level=$(echo "$RDP_level" | tr -d '\n\r')
    case "$RDP_level" in
        "0")
            rdp_value="0xAA"
            rdp_str="OB_RDP_LEVEL_0"
            ;;
        "1")
            rdp_value="0xBB"
            rdp_str="OB_RDP_LEVEL_1"
            ;;
        "2")
            rdp_value="0xCC"
            rdp_str="OB_RDP_LEVEL_2"
            ;;
        *)
            echo "WRONG RDP level selected (send $clean_var)"
            current_log_file="./*.log files "
            define_rdp_level
            ;;
    esac
}

define_data_size() {
    action="Define data area size in Kbytes"
    echo "* $action"
    read -p "      ${USERREG} [ 0 | 8 | 16 | 24 | 32 | 40 | 48 | 56 | 64 ]: " data_size
	data_size=$(echo "$data_size" | tr -d '\n\r')
    case "$data_size" in
        "0")
            echo ""
            data_hex_val="0x0"
            subregion_val="0xFF"
            ;;
        "8")
            echo ""
            data_hex_val="0x2000"
            subregion_val="0x7F"
            ;;
        "16")
            echo ""
            data_hex_val="0x4000"
            subregion_val="0x3F"
            ;;
        "24")
            echo ""
            data_hex_val="0x6000"
            subregion_val="0x1F"
            ;;
        "32")
            echo ""
            data_hex_val="0x8000"
            subregion_val="0xF"
            ;;
        "40")
            echo ""
            data_hex_val="0xA000"
            subregion_val="0x7"
            ;;
        "48")
            echo ""
            data_hex_val="0xC000"
            subregion_val="0x3"
            ;;
        "56")
            echo ""
            data_hex_val="0xE000"
            subregion_val="0x1"
            ;;
        "64")
            echo ""
            data_hex_val="0x10000"
            subregion_val="0x0"
            ;;
        *)
            echo "Invalid input. Please enter a valid option  (send $clean_var)."
            define_data_size
            ;;
    esac
}

# Application binary file
appli_binary="${projectdir}/../Applications/ROT/OEMiSB_Appli/Binary/OEMiSB_Appli.bin"
# Sha.bin
sha256="${projectdir}/OEMiSB/Binary/sha256.bin"

# Data updated with the postbuild of OEMiSB-Boot
hdp_end=0x55"0x3"

bootaddress="0x8000000"
appliaddress="0x8002000"
shaaddress="0x8001FC0"
# CubeProgammer connection
connect_no_reset="-c port=SWD mode=Hotplug"
connect_reset="-c port=SWD mode=UR"

# =============================================== Remove protections and initialize Option Bytes  ==========================================
remove_protect_init="-ob WRP1A_STRT=0x7f WRP1A_END=0 HDP1_PEND=0 HDP1EN=0xB4 BOOT_LOCK=0"
# =============================================== Erase the user flash =====================================================================
erase_all="-e all"

# ================================================ hardening ===============================================================================

hide_protect="HDP1_PEND=${hdp_end} HDP1EN=1"

boot_lock="BOOT_LOCK=1"

isGeneratedByCubeMX="${PROJECT_GENERATED_BY_CUBEMX}"
# CubeProgammer path and input files
state_change_log="provisioning.log"

appli_main_h="${cube_fw_path}/Projects/NUCLEO-U083RC/Applications/ROT/OEMiSB_Appli/Inc/main.h"
boot_main_h="${cube_fw_path}/Projects/NUCLEO-U083RC/Applications/ROT/OEMiSB_Boot/Inc/main.h"
boot_cfg_h="${cube_fw_path}/Projects/NUCLEO-U083RC/Applications/ROT/OEMiSB_Boot/Inc/boot_cfg.h"
ld_appli="${cube_fw_path}/Projects/NUCLEO-U083RC/Applications/ROT/OEMiSB_Appli/STM32CubeIDE/STM32U083RCIX_FLASH.ld"

applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/dist/AppliCfg.exe"

#line for python
echo AppliCfg with python script
applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"
#determine/check python version command
python="python "


echo "======"
echo "====== Provisioning of OEMiSB boot path"
echo "====== Application selected through env.bat:"
echo "====== ${oemisb_boot_path_project}"
echo "======"

# ============================================================= RDP selection ==============================================================
define_rdp_level
# ========================================================= Data size selection ==============================================================
define_data_size

tmp_file="${projectdir}/tmp.c"
echo "DATA_SIZE=${data_hex_val}" > "${tmp_file}"
echo "DATA_MPU_SUB_REG=${subregion_val}" >> "${tmp_file}"
$python$applicfg definevalue -l "${tmp_file}" -m DATA_SIZE -n DATA_SIZE "${boot_main_h}"
$python$applicfg definevalue -l "${tmp_file}" -m DATA_SIZE -n DATA_SIZE "${appli_main_h}"
$python$applicfg definevalue -l "${tmp_file}" -m DATA_MPU_SUB_REG -n DATA_MPU_SUB_REG "${boot_main_h}"
$python$applicfg linker -l "${tmp_file}" -m DATA_SIZE -n FLASH_DATA_AREA_SIZE "${ld_appli}"
$python$applicfg modifyfilevalue --variable OEMISB_OB_RDP_LEVEL_VALUE --value "${rdp_str}" "${boot_cfg_h}" --str

wrpend=$((0x7F - data_hex_val/0x800))
write_protect="WRP1A_STRT=0 WRP1A_END=${wrpend}"

echo "Step 2 : Projects generation"
echo "    * Boot project generation"
echo "        Open the OEMiSB_Boot project with preferred toolchain and rebuild all files."
echo "        Press any key to continue..."
if [ "$mode" != "AUTO" ]; then read -p "" -s; fi
echo ""


echo "    * Application project generation"
echo "        Open the OEMiSB_Appli project with preferred toolchain and rebuild all files."
echo "        Press any key to continue..."
if [ "$mode" != "AUTO" ]; then read -p "" -s; fi
echo ""

echo "Step 3 : Product programming"
action="Programming the option bytes and flashing binaries ..."
current_log_file="$ob_flash_log"

action="Remove protection and flash erase"
echo "    * $action"
echo """$stm32programmercli""" $connect_reset $remove_protect_init $erase_all > "$state_change_log"
"$stm32programmercli" $connect_reset $remove_protect_init $erase_all >> "$state_change_log"
if [ $? -ne 0 ]; then
    echo "Error: Failed to remove protection and erase flash."
    step_error
fi

echo ""
echo "    * Project flash programming"
echo "        OEMiSB application programming"
action="OEMiSB_Appli Written"
"$stm32programmercli" "$connect_reset" -d "$cube_fw_path/Projects/NUCLEO-U083RC/Applications/ROT/OEMiSB_Appli/Binary/OEMiSB_Appli.bin" "$appliaddress" -v >> "$state_change_log"
if [ $? -ne 0 ]; then
    echo "Error: Failed to program OEMiSB_Appli."
    step_error
fi
echo "        - $action"
echo "$action" >> ob_flash_programming.log

action="OEMiSB application SHA256 programming"
echo "        - $action"
$python$applicfg hashcontent tmp.bin -i "$appli_binary" -t "10000" -d "$sha256" --create
"$stm32programmercli" "$connect_reset" -d "$cube_fw_path/Projects/NUCLEO-U083RC/ROT_Provisioning/OEMiSB/Binary/sha256.bin" "$shaaddress" -v >> "$state_change_log"
if [ $? -ne 0 ]; then
    echo "Error: Failed to program OEMiSB application SHA256."
    step_error
fi
action="SHA Appli Written"
echo "            - $action"
echo "$action" >> ob_flash_programming.log

action="OEMiSB boot programming"
echo "        - $action"
"$stm32programmercli" "$connect_reset" -d "$cube_fw_path/Projects/NUCLEO-U083RC/Applications/ROT/OEMiSB_Boot/Binary/OEMiSB_Boot.bin" "$bootaddress" -v >> "$state_change_log"
if [ $? -ne 0 ]; then
    echo "Error: Failed to program OEMiSB boot."
    step_error
fi

action="OEMiSB_Boot Written"
echo "            - $action"
echo "$action" >> "$state_change_log"

echo ""
echo "    * Configure Option Bytes:"
echo "        - Write Protection"
echo "        - Hide Protection"
echo "        - Boot Lock"
"$stm32programmercli" "$connect_reset" -ob "$write_protect" "$hide_protect" "$boot_lock" >> "$state_change_log"
if [ $? -ne 0 ]; then
    echo "Error: Failed to configure option bytes."
    step_error
fi
echo ""

action="Configure Option Bytes Done"
echo "        - $action"
echo "$action" >> "$state_change_log"

action="Setting the final RDP Level $RDP_level"
echo "    * $action"
#"$stm32programmercli" "$connect_no_reset" -ob RDP="$rdp_value" >> "$state_change_log"
# cub prog issue: not able to return to level0 from level1
# Error: Expected value for Option Byte "rdp": 0xAA, found: 0x0
# Error: Option Byte Programming failed
echo ""
final_execution