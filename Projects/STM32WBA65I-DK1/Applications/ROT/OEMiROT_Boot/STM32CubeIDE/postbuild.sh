#!/bin/bash
#=================================================================================================
# Managing HOST OS diversity : begin
#=================================================================================================
OS=$(uname)
#
echo ${OS} | grep -i -e windows -e mingw >/dev/null
if [ $? == 0 ]; then
  echo "=================================="
  echo "HOST OS : Windows detected"
  echo ""
  echo ">>> Running ../postbuild.bat $@"
  echo ""
  # Enable : exit immediately if any commands returns a non-zero status
  set -e
  cd ../
  cmd.exe /C postbuild.bat $@
  # Return OK if no error detected during .bat script
  exit 0
fi
#
if [ "$OS" == "Linux" ]; then
  echo "HOST OS : Linux detected"
elif [ "$OS" == "Darwin" ]; then
  echo "HOST OS : MacOS detected"
else
  echo "!!!HOST OS not supported : >$OS<!!!"
  exit 1
fi
#
#=================================================================================================
# Managing HOST OS diversity : end
#=================================================================================================
echo "=================================="
echo ">>> Running $0 $@"
echo ""
#
# arg1 is the config type (Debug, Release)
config=$1
#
# Getting the Trusted Package Creator CLI path
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`
cd "$project_dir/../../../../ROT_Provisioning"
provisioningdir=$(pwd)
cd $project_dir

source $provisioningdir/env.sh "$provisioningdir"
source $provisioningdir/OEMiROT/img_config.sh "$provisioningdir"

if [ "$oemurot_enabled" == "1" ]; then
  project=OEMuROT
  bootpath=OEMiRoT_OEMuRoT

  source "$provisioningdir/OEMiRoT_OEMuRoT/img_config.sh"
else
  project=OEMiROT
  bootpath=OEMiROT
fi

# Environment variable for log file
export current_log_file="$project_dir/postbuild.log"
echo "" > "$current_log_file"

preprocess_bl2_file="$project_dir/image_macros_preprocessed_bl2.c"

appli_dir="${PWD}/../../../../${oemirot_appli_path_project}"
postbuild_appli="$appli_dir/STM32CubeIDE/postbuild.sh"
appli_flash_layout="$appli_dir/Secure_nsclib/appli_flash_layout.h"

ob_flash_programming="$provisioningdir/$bootpath/ob_flash_programming.sh"
img_config="$provisioningdir/$bootpath/img_config.sh"
regression="$provisioningdir/$bootpath/regression.sh"

#=================================================================================================
#image xml configuration files
#=================================================================================================
s_code_xml="$provisioningdir/$bootpath/Images/${project}_S_Code_Image.xml"
ns_code_xml="$provisioningdir/$bootpath/Images/${project}_NS_Code_Image.xml"
s_code_init_xml="$provisioningdir/$bootpath/Images/${project}_S_Code_Init_Image.xml"
ns_code_init_xml="$provisioningdir/$bootpath/Images/${project}_NS_Code_Init_Image.xml"
s_data_xml="$provisioningdir/$bootpath/Images/${project}_S_Data_Image.xml"
ns_data_xml="$provisioningdir/$bootpath/Images/${project}_NS_Data_Image.xml"
s_data_init_xml="$provisioningdir/$bootpath/Images/${project}_S_Data_Init_Image.xml"
ns_data_init_xml="$provisioningdir/$bootpath/Images/${project}_NS_Data_Init_Image.xml"

if [ "$oemurot_enabled" == "1" ]; then
  oemurot_code_xml="$provisioningdir/OEMiRoT_OEMuRoT/Images/OEMiROT_Code_Image.xml"
  oemurot_code_init_xml="$provisioningdir/OEMiRoT_OEMuRoT/Images/OEMiROT_Code_Init_Image.xml"
fi

#=================================================================================================
# Variables for image xml configuration(ROT_Provisioning\%bootpath%\Images)
# relative path from ROT_Provisioning\%bootpath%\Images directory to retrieve binary files
#=================================================================================================
auth_s_xml_field="Authentication secure key"
auth_ns_xml_field="Authentication non secure key"
scratch_sector_number_xml_field="Number of scratch sectors"

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
#
# Function to run commands and check for errors
error()
{
    echo ""
    echo "====="
    echo "===== Error occurred."
    echo "===== See $current_log_file for details. Then try again."
    echo "====="
    exit 1
}

# postbuild
# =============================================================== Generate OEMuROT image ================================================================
if [ "$oemurot_enabled" == "1" ]; then
  echo ""
  echo "Creating OEMuROT image"
  echo ""
  echo "Creating OEMuROT image" >> $current_log_file 2>&1
  "$stm32tpccli" -pb $oemurot_code_xml >> $current_log_file 2>&1
  if [ $? != 0 ]; then error; fi

  "$stm32tpccli" -pb $oemurot_code_init_xml >> $current_log_file 2>&1
  if [ $? != 0 ]; then error; fi
fi

# ============================================================ Update %img_config% ============================================================
$python$applicfg flash --layout $preprocess_bl2_file -b first_boot_stage -m RE_OEMIROT_FIRST_BOOT_STAGE --decimal --vb $img_config >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b app_image_number -m RE_APP_IMAGE_NUMBER --decimal --vb $img_config >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b s_data_image_number -m RE_S_DATA_IMAGE_NUMBER --decimal --vb $img_config >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b ns_data_image_number -m RE_NS_DATA_IMAGE_NUMBER --decimal --vb $img_config >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b app_full_secure -m  RE_OEMIROT_APPLI_FULL_SECURE --decimal --vb $img_config >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b external_flash -m RE_EXTERNAL_FLASH_ENABLE --decimal --vb $img_config >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

source $img_config

if [ "$first_boot_stage" == "1" ]; then
echo "OEMiROT_Boot is generated for OEMIROT_FIRST_BOOT_STAGE. Do not change any scripts!"
echo "copying ../Binary/OEMiROT_Boot.bin in %provisioningdir%/OEMiRoT_OEMuRoT/Binary/"
cp ../Binary/OEMiROT_Boot.bin $provisioningdir/OEMiRoT_OEMuRoT/Binary
exit 0
fi

# ============================================================ Update %regression% =====================================================================
$python$applicfg flash --layout $preprocess_bl2_file -b secbootadd0 -m  RE_BL2_BOOT_ADDRESS  -d 0x80 $regression --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b flashsectnbr -m  RE_FLASH_PAGE_NBR $regression --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b select_wba_target -m  RE_SELECT_WBA_TARGET --vb $regression>> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

# ============================================================ Update %ob_flash_programming% ===========================================================
$python$applicfg flash --layout $preprocess_bl2_file -b keyaddress -m  RE_BL2_PERSO_ADDRESS $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b bootaddress -m  RE_BL2_BOOT_ADDRESS $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b oemirot_oemurot_slot0 -m  RE_OEMIROT_DOWNLOAD_IMAGE_AREA_0 $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b secbootadd0 -m RE_BL2_BOOT_ADDRESS -d 0x80 $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b flashsectnbr -m RE_FLASH_PAGE_NBR $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b slot0 -m RE_IMAGE_FLASH_ADDRESS_SECURE $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b slot1 -m RE_IMAGE_FLASH_ADDRESS_NON_SECURE $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b slot2 -m RE_IMAGE_FLASH_SECURE_UPDATE $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b slot3 -m RE_IMAGE_FLASH_NON_SECURE_UPDATE $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b slot4 -m RE_IMAGE_FLASH_ADDRESS_DATA_SECURE $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b slot5 -m RE_IMAGE_FLASH_ADDRESS_DATA_NON_SECURE $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b slot6 -m RE_IMAGE_FLASH_DATA_SECURE_UPDATE $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b slot7 -m RE_IMAGE_FLASH_DATA_NON_SECURE_UPDATE $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b wrp_a_bank1_start -m RE_BL2_WRP_START -d 0x2000 $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b wrp_a_bank1_end -m RE_BL2_WRP_END -d 0x2000 $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b sec1_start -m RE_BL2_SEC1_START -d 0x1000 $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b sec1_end -m RE_BL2_SEC1_END -d 0x2000 $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b hdp_end -m RE_BL2_HDP_END -d 0x2000 $ob_flash_programming --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg flash --layout $preprocess_bl2_file -b select_wba_target -m RE_SELECT_WBA_TARGET --vb $ob_flash_programming >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

if [ "$oemurot_enabled" == "1" ]; then
  $python$applicfg flash --layout $preprocess_bl2_file -b is_wba_board_dk -m RE_IS_WBA_BOARD_DK --decimal --vb $ob_flash_programming >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi

  $python$applicfg flash --layout $preprocess_bl2_file -b overwrite -m RE_OVER_WRITE --decimal --vb $ob_flash_programming >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi
fi

# ============================================================ Update %postbuild_appli% ==============================================================
if [ "$app_full_secure" == "0" ]; then
  $python$applicfg flash --layout $preprocess_bl2_file -b image_s_size -m  RE_IMAGE_FLASH_SECURE_IMAGE_SIZE $postbuild_appli --vb >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi

  $python$applicfg flash --layout $preprocess_bl2_file -b image_ns_size -m  RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE $postbuild_appli --vb >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi

  $python$applicfg flash --layout $preprocess_bl2_file -b app_image_number -m  RE_APP_IMAGE_NUMBER --decimal $postbuild_appli --vb >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi
fi

# ============================================================ Update %ns_code_xml% ==================================================================
if [ "$app_full_secure" == "0" ]; then
  $python$applicfg xmlname --layout $preprocess_bl2_file -m RE_APP_IMAGE_NUMBER -n "$auth_ns_xml_field" -sn "$auth_s_xml_field" -v 1 -c k $ns_code_xml --vb >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi

  $python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE -c S $ns_code_xml --vb >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi

  $python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" $ns_code_xml --vb >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi

  $python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" $ns_code_xml --vb >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi

  $python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE -c S $ns_code_init_xml --vb >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi
fi

# ============================================================ Update %s_code_xml% ===================================================================
$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -c S $s_code_xml --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" $s_code_xml --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" $s_code_xml --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -c S $s_code_init_xml --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

# ============================================================ Update %ns_data_xml% ==================================================================
if [ "$app_full_secure" == "0" ]; then
  $python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -c S $ns_data_xml --vb >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi

  $python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" $ns_data_xml --vb >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi

  $python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" $ns_data_xml --vb >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then  error; fi

  $python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -c S $ns_data_init_xml --vb >> $current_log_file 2>&1
  ret=$?
  if [ $ret != 0 ]; then error; fi
fi

# ============================================================ Update %s_data_xml% ===================================================================
$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -c S $s_data_xml --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_ENCRYPTION -n "Encryption key" -link GetPublic -t File -c -E -h 1 -d "" $s_data_xml --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg xmlparam --layout  $preprocess_bl2_file -m RE_OVER_WRITE -n "Write Option" -t Data -c --overwrite-only -h 1 -d "" $s_data_xml --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg xmlval --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -c S $s_data_init_xml --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

# ============================================================ Update %appli_flash_layout% ===========================================================
$python$applicfg setdefine --layout $preprocess_bl2_file -m RE_OVER_WRITE -n MCUBOOT_OVERWRITE_ONLY -v 1 $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg setdefine --layout $preprocess_bl2_file -m RE_EXTERNAL_FLASH_ENABLE -n OEMIROT_EXTERNAL_FLASH_ENABLE -v 1 $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_APP_IMAGE_NUMBER -n MCUBOOT_APP_IMAGE_NUMBER $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_S_DATA_IMAGE_NUMBER -n MCUBOOT_S_DATA_IMAGE_NUMBER $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_NS_DATA_IMAGE_NUMBER -n MCUBOOT_NS_DATA_IMAGE_NUMBER $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_0_OFFSET -n FLASH_AREA_0_OFFSET $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_0_SIZE -n FLASH_AREA_0_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_1_OFFSET -n FLASH_AREA_1_OFFSET $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_1_SIZE -n FLASH_AREA_1_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_2_OFFSET -n FLASH_AREA_2_OFFSET $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_2_SIZE -n FLASH_AREA_2_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_3_OFFSET -n FLASH_AREA_3_OFFSET $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_3_SIZE -n FLASH_AREA_3_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_4_OFFSET -n FLASH_AREA_4_OFFSET $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_4_SIZE -n FLASH_AREA_4_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_5_OFFSET -n FLASH_AREA_5_OFFSET $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_5_SIZE -n FLASH_AREA_5_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_6_OFFSET -n FLASH_AREA_6_OFFSET $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_6_SIZE -n FLASH_AREA_6_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_7_OFFSET -n FLASH_AREA_7_OFFSET $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_AREA_7_SIZE -n FLASH_AREA_7_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_S_NS_PARTITION_SIZE -n FLASH_PARTITION_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_NON_SECURE_IMAGE_SIZE -n FLASH_NS_PARTITION_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_IMAGE_SIZE -n FLASH_S_PARTITION_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_SECURE_DATA_IMAGE_SIZE -n FLASH_S_DATA_PARTITION_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_IMAGE_FLASH_NON_SECURE_DATA_IMAGE_SIZE -n FLASH_NS_DATA_PARTITION_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_FLASH_B_SIZE -n FLASH_B_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_OEMIROT_DOWNLOAD_IMAGE_AREA_0_OFFSET -n OEMIROT_AREA_0_OFFSET $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_OEMIROT_DOWNLOAD_IMAGE_AREA_0_SIZE -n OEMIROT_AREA_0_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_OEMIROT_DOWNLOAD_IMAGE_AREA_2_OFFSET -n OEMIROT_AREA_2_OFFSET $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_OEMIROT_DOWNLOAD_IMAGE_AREA_2_SIZE -n OEMIROT_AREA_2_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

if [ "$oemurot_enabled" == "1" ]; then
  $python$applicfg setdefine -a uncomment -n OEMUROT_ENABLE -v 1 $appli_flash_layout
  ret=$?
else
  $python$applicfg setdefine -a comment -n OEMUROT_ENABLE -v 1 $appli_flash_layout
  ret=$?
fi
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_BOOT_SHARED_DATA_BASE -n BOOT_SHARED_DATA_BASE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_BOOT_SHARED_DATA_SIZE -n BOOT_SHARED_DATA_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_BOOT_RAM_BASE -n BOOT_RAM_BASE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

$python$applicfg definevalue --layout $preprocess_bl2_file -m RE_BOOT_RAM_SIZE -n BOOT_RAM_SIZE $appli_flash_layout --vb >> $current_log_file 2>&1
ret=$?
if [ $ret != 0 ]; then error; fi

# ======================================================================== end =======================================================================
exit 0

