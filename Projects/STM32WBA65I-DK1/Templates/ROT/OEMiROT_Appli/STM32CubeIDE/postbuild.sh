#!/bin/bash -
# arg1 is the binary type (1 nonsecure, 2 secure)
signing=$1
# arg2 is the config type (Debug, Release)
config=$2

# Getting the Trusted Package Creator CLI path
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`
cd "$project_dir/../../../../ROT_Provisioning"
provisioningdir=$(pwd)
cd $project_dir

source $provisioningdir/env.sh "$provisioningdir"
source $provisioningdir/OEMiROT/img_config.sh

if [ "$oemurot_enabled" == "1" ]; then
  project=OEMuROT
  bootpath=OEMiRoT_OEMuRoT
else
  project=OEMiROT
  bootpath=OEMiROT
fi

error()
{
    echo ""
    echo "====="
    echo "===== Error occurred."
    echo "===== See $current_log_file for details. Then try again."
    echo "====="
    exit 1
}

# Environment variable for log file
current_log_file="$project_dir/postbuild.log"
echo "" > $current_log_file

#=================================================================================================
#image xml configuration files
#=================================================================================================
s_code_xml="$provisioningdir/$bootpath/Images/${project}_S_Code_Image.xml"
s_code_init_xml="$provisioningdir/$bootpath/Images/${project}_S_Code_Init_Image.xml"
s_data_xml="$provisioningdir/$bootpath/Images/${project}_S_Data_Image.xml"
s_data_init_xml="$provisioningdir/$bootpath/Images/${project}_S_Data_Init_Image.xml"

#=================================================================================================
#Variables for image xml configuration(ROT_Provisioning/%bootpath%/Images)
#relative path from ROT_Provisioning/%bootpath%/Images directory to retrieve binary files
#=================================================================================================
bin_path_xml_field="../../../Templates/ROT/OEMiROT_Appli/Binary"
data_path_xml_field="../Binary"
fw_in_bin_xml_field="Firmware binary input file"
fw_out_bin_xml_field="Image output file"
s_app_bin_xml_field="$bin_path_xml_field/oemirot_tz_s_app.bin"
s_data_enc_sign_bin_xml_field="$data_path_xml_field/s_data_enc_sign.bin"
s_data_init_sign_bin_xml_field="$data_path_xml_field/s_data_init_sign.bin"

if [ "$oemurot_enabled" == "1" ]; then
  s_app_enc_sign_bin_xml_field="$bin_path_xml_field/oemurot_tz_s_app_enc_sign.bin"
  s_app_init_sign_bin_xml_field="$bin_path_xml_field/$oemurot_appli_secure"
else
  s_app_enc_sign_bin_xml_field="$bin_path_xml_field/oemirot_tz_s_app_enc_sign.bin"
  s_app_init_sign_bin_xml_field="$bin_path_xml_field/$oemirot_appli_secure"
fi

applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/dist/AppliCfg.exe"
uname | grep -i -e windows -e mingw
if [ $? == 0 ] && [ -e "$applicfg" ]; then
  #line for window executable
  echo "AppliCfg with windows executable"
  python=""
else
  #line for python
  echo "AppliCfg with python script"
  applicfg="$cube_fw_path/Utilities/PC_Software/ROT_AppliConfig/AppliCfg.py"
  #determine/check python version command
  python="python3 "
fi

#postbuild
echo "Postbuild $signing image" >> $current_log_file 2>&1

if  [ $signing == "secure" ]; then
    #update xml file : input file
    $python$applicfg xmlval -v $s_app_bin_xml_field --string -n "$fw_in_bin_xml_field" $s_code_xml --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi

    #update xml file : output file
    $python$applicfg xmlval -v $s_app_enc_sign_bin_xml_field --string -n "$fw_out_bin_xml_field" $s_code_xml --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi

    echo Creating secure image  >> $current_log_file 2>&1
    "$stm32tpccli" -pb $s_code_xml >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi

    #update xml file : input file
    $python$applicfg xmlval -v $s_app_bin_xml_field --string -n "$fw_in_bin_xml_field" $s_code_init_xml --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi

    #update xml file : output file
    $python$applicfg xmlval -v $s_app_init_sign_bin_xml_field --string -n "$fw_out_bin_xml_field" $s_code_init_xml --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi

    "$stm32tpccli" -pb $s_code_init_xml >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi

    #update xml file : output file
    $python$applicfg xmlval -v $s_data_enc_sign_bin_xml_field --string -n "$fw_out_bin_xml_field" $s_data_xml --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi

    $python$applicfg xmlval -v $s_data_init_sign_bin_xml_field --string -n "$fw_out_bin_xml_field" $s_data_init_xml --vb >> $current_log_file 2>&1
    if [ $? != 0 ]; then error; fi
fi

exit 0
