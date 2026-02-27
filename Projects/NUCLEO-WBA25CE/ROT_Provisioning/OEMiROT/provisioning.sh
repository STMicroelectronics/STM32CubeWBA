#!/bin/bash
source ../env.sh

# Getting the Trusted Package Creator CLI path
if [ $# -ge 1 ]; then mode=$1; else mode=MANUAL; fi

# Environment variable for AppliCfg
SCRIPT=$(readlink -f $0)
projectdir=`dirname $SCRIPT`
oemirot_config_xml="$projectdir/Config/OEMiRoT_Config.xml"

# CubeProgammer path and input files
ob_flash_programming="ob_flash_programming.sh"

# OEM2 key path
oem2_password="./Keys/oem2_password.txt"

# Log files
ob_flash_log="ob_flash_programming.log"
provisioning_log="provisioning.log"

# Environment variable for path project
appli_dir=../../$oemirot_appli_path_project
boot_dir=../../$oemirot_boot_path_project

# Get config updated by OEMiROT_Boot
img_config=$projectdir/img_config.sh

s_data_xml="$projectdir/Images/OEMiROT_S_Data_Image.xml"
ns_data_xml="$projectdir/Images/OEMiROT_NS_Data_Image.xml"
s_data_init_xml="$projectdir/Images/OEMiROT_S_Data_Init_Image.xml"
ns_data_init_xml="$projectdir/Images/OEMiROT_NS_Data_Init_Image.xml"

boot_cfg_h="$cube_fw_path/Projects/NUCLEO-WBA25CE/Applications/ROT/OEMiROT_Boot/Inc/boot_hal_cfg.h"
flash_layout="$cube_fw_path/Projects/NUCLEO-WBA25CE/Applications/ROT/OEMiROT_Boot/Inc/flash_layout.h"

# Initial configuration
connect_no_reset="-c port=SWD mode=HotPlug"
connect_reset="-c port=SWD mode=UR"

# Check if Python is installed
if ! python3 --version > /dev/null 2>&1; then
  if ! python --version > /dev/null 2>&1; then
    echo "Python installation missing. Refer to Utilities/PC_Software/ROT_AppliConfig/README.md"
    step_error;
  fi
  python="python "
else
  python="python3 "
fi

# Environment variable for AppliCfg
applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"

# Display Boot path
echo "====="
echo "===== Provisioning of OEMiRoT boot path"
echo "===== Application selected through env.sh:"
echo "=====" $oemirot_appli_path_project
echo -e "\e[31m===== Python and some python packages are required to execute this script: Refer to\e[0m"
echo -e "\e[31m===== Utilities/PC_Software/ROT_AppliConfig/README.md for more details\e[0m"
echo "====="
echo

# Check if boot path exists
if [ ! -d "$boot_dir" ]; then
echo "====="
echo "===== Wrong Boot path: $oemirot_boot_path_project"
echo "===== please modify the env.sh to set the right path"
step_error;
fi

# Check if oem2 password path exists
if [ ! -f "$oem2_password" ]; then
echo "====="
echo "===== Wrong OEM 2 password path: $oem2_password"
echo "===== please put the right path"
step_error;
fi

# ============================================================= RDP selection =============================================================
define_rdp_level()
{
    action="Define RDP level"
    echo "   * $action"
    read -p "     [ 0 | 1 | 2 ]: " RDP_level
    RDP_level=$(echo "$RDP_level" | tr -d '\n\r')

    if [ "$RDP_level" == "0" ]; then
        echo
        rdp_value=0xAA
        rdp_str="OB_RDP_LEVEL_0"
        return
    fi

    if [ "$RDP_level" == "1" ]; then
        echo
        rdp_value=0xBB
        rdp_str="OB_RDP_LEVEL_1"
        return
    fi

    if [ "$RDP_level" == "2" ]; then
        echo
        rdp_value=0xCC
        rdp_str="OB_RDP_LEVEL_2"
        set_oem2_key  # OEM2 key Setting
        return
    fi

    echo "       WRONG RDP level selected"
    echo
    define_rdp_level
}

# ============================================================ OEM2 key Setting ==========================================================
set_oem2_key()
{
    # Step to configure OEM2 key
    echo "   * OEM2 key setup"
    echo "       Open oem2_password file and put OEM2 key(Default path is \ROT_Provisioning\OEMiRoT\Keys\oem2_password.txt)"
    echo "       Warning: Default OEM2 keys must NOT be used in a product. Make sure to regenerate your own OEM2 keys!"
    echo "       Press any key to continue..."
    echo
    if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
}

# =============================================== Steps to generate OEMiRoT_Keys.bin file =================================================
binary_keys_generation()
{
    echo "   * OEMiROT keys configuration"
    echo "       From TrustedPackageCreator (OBkey tab in Security panel)"
    echo "       Select OEMiRoT_Config.xml(Default path is /ROT_Provisioning/OEMiROT/Config/OEMiRoT_Config.xml)"
    echo "       Warning: Default keys must NOT be used in a product. Make sure to regenerate your own keys!"
    echo "       Update the configuration (if/as needed) then generate OEMiRoT_Keys.bin file"
    echo "       Press any key to continue..."
    echo
    if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

    "$stm32tpccli" -obk $oemirot_config_xml >> $provisioning_log 2>&1
    if [ $? != 0 ]; then step_error; fi
}

# ========================================================= Images generation steps =======================================================
images_generation()
{
    echo "   * Boot firmware image generation"
    echo "       Open the OEMiROT_Boot project with preferred toolchain and rebuild all files."
    echo "       At this step the project is configured for OEMiROT boot path."
    echo "       Press any key to continue..."
    echo
    if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
    source $img_config

    echo "   * Code firmware image generation"
    if [ "$app_full_secure" == "1" ]; then
      echo "       Open the OEMiROT_Appli project with preferred toolchain."
      echo "       Rebuild the Secure project. The $oemirot_appli_secure and oemirot_tz_s_app_enc_sign.bin files are generated with the postbuild command."
    else
      echo "       Open the OEMiROT_Appli_TrustZone project with preferred toolchain."
      echo "       Rebuild the Secure project. The $oemirot_appli_secure and oemirot_tz_s_app_enc_sign.bin files are generated with the postbuild command."
      echo "       Rebuild the NonSecure project. The $oemirot_appli_non_secure and oemirot_tz_ns_app_enc_sign.bin files are generated with the postbuild command."
    fi
    echo "       Press any key to continue..."
    echo
    if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

    echo "   * Data secure generation (if Data secure image is enabled)"
    echo "       Select OEMiRoT_S_Data_Image.xml(Default path is /ROT_Provisioning/OEMiROT/Images/OEMiROT_S_Data_Image.xml)"
    echo "       Generate the data_enc_sign.bin image"
    echo "       Press any key to continue..."
    echo
    if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

    if [ $s_data_image_number != "0" ]; then
        "$stm32tpccli" -pb $s_data_xml >> $provisioning_log 2>&1
        if [ $? != 0 ]; then step_error; fi

        "$stm32tpccli" -pb $s_data_init_xml >> $provisioning_log 2>&1
        if [ $? != 0 ]; then step_error; fi
    fi

    if [ "$app_full_secure" != "1" ]; then
        echo "   * Data non secure generation (if Data non secure image is enabled)"
        echo "       Select OEMiROT_NS_Data_Image.xml(Default path is /ROT_Provisioning/OEMiROT/Images/OEMiROT_NS_Data_Image.xml)"
        echo "       Generate the data_enc_sign.bin image"
        echo "       Press any key to continue..."
        echo
        if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

        if [ $ns_data_image_number != "0" ]; then
            "$stm32tpccli" -pb $ns_data_xml >> $provisioning_log 2>&1
            if [ $? != 0 ]; then step_error; fi

            "$stm32tpccli" -pb $ns_data_init_xml >> $provisioning_log 2>&1
            if [ $? != 0 ]; then step_error; fi
        fi
    fi
}

# Clean OEMuROT configuration
clean_oemurot_cfg()
{
    $python$applicfg definevalue --name=oemurot_enabled  --value=0 --decimal $img_config
    ret=$?
    if [ $ret != "0" ]; then step_error; fi

    $python$applicfg setdefine -a comment -n OEMUROT_ENABLE -v 1 $flash_layout
    ret=$?
    if [ $ret != "0" ]; then step_error; fi
}

# ================================================== Option Bytes and flash programming ===================================================
# Step to update RDP Level in boot_hal_cfg
change_rdp_level_in_boot_cfg()
{
    # Replace RDP level in boot_hal_cfg
    $python$applicfg modifyfilevalue --variable OEMIROT_OB_RDP_LEVEL_VALUE --value "${rdp_str}" "${boot_cfg_h}" --str
    ret=$?
    if [ $ret != "0" ]; then step_error; fi
}

oem2_key_provisioning()
{
    # Get OEM2 key
    read -r oem2_key < "$oem2_password"

    "$stm32programmercli" $connect_reset -hardRst -lockRDP2 $oem2_key >> $provisioning_log 2>&1
    if [ $? != 0 ]; then step_error; fi
    echo "   * Provisioning of OEM2 key Done"
    echo
}

ob_programming()
{
    action="Programming the option bytes and flashing the images..."
    current_log_file=$ob_flash_log
    command="source $ob_flash_programming AUTO $RDP_level"
    echo "   * $action"
    $command > "$current_log_file"

    ob_flash_error=$?

    if [ $ob_flash_error -ne 0 ]; then step_error; fi

    echo "       Successful option bytes programming and images flashing"
    echo "       (see $current_log_file for details)"
    echo
}

# =========================================================== RDP level setting ===========================================================
set_rdp_level()
{
    action="Setting RDP level $RDP_level"
    current_log_file=$provisioning_log
    echo "   * $action"
    "$stm32programmercli" $connect_no_reset -ob RDP=$rdp_value >> $provisioning_log 2>&1
    echo
    if [ $? -ne 0 ]; then step_error; fi
    if [ "$rdp_value" != "0xAA" ]; then
      echo "   *Please Unplug USB Cable and Plug it again to recover SWD connection"
      echo "        Press any key to continue..."
      if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi
    fi
}

# ============================================================= End functions =============================================================
# Error when external script is executed
step_error()
{
  echo
  echo "====="
  echo "===== Error while executing "$action"."
  echo "===== See $current_log_file for details. Then try again."
  echo "====="
  if [ "$mode" != "AUTO" ]; then $SHELL; fi
  exit 1
}

# ============================================================== Script start =============================================================

# ========================================================= Configuration management=======================================================
echo "Step 1 : Configuration management"
define_rdp_level

change_rdp_level_in_boot_cfg

# ======================================================== Clean OEMuROT configuration ====================================================
clean_oemurot_cfg

# =============================================== Steps to generate OEMiRoT_Keys.bin file =================================================
binary_keys_generation

# ========================================================= Images generation steps =======================================================
echo "Step 2 : Images generation"
images_generation

# ============================================================== Provisioning steps =======================================================
echo "Step 3 : Provisioning"

echo "   * BOOT0 pin should be disconnected from VDD"
echo "       Press any key to continue..."
echo ""
if [ "$mode" != "AUTO" ]; then read -p "" -n1 -s; fi

# ============================================================ OEM2 key provisioning ======================================================
oem2_key_provisioning

# ================================================== Option Bytes and flash programming ===================================================
ob_programming

# =========================================================== RDP level setting ===========================================================
set_rdp_level

# ============================================================= End functions =============================================================
echo "====="
echo "===== The board is correctly configured."
echo "===== Connect UART console (115200 baudrate) to get application menu."
echo "====="
if [ "$mode" != "AUTO" ]; then $SHELL; fi
exit 0

