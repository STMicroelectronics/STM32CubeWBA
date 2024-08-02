#!/bin/bash -
# arg1 is the build directory
# arg2 is the version
# arg3 is the binary type (1 nonsecure, 2 secure)
# arg4 dependency with the other image
# "1.0.0" : version dependency with other image
# "nodep": no dependency
# arg5 is version and dependency in output filename
# "version" : filename contains the version and dependency
# arg6 is to force python execution
# "python" : execute with python script

# Absolute path to this script
SCRIPT=$(readlink -f $0)
projectdir=`dirname $SCRIPT`

version=$2
signing=$3
dep="nodep"
dep_name=""
vername="noversion"
# default option value
option="-e little -H 0x400 --pad-header -s auto --align 8"
if [ $# -eq 6 ] || [ $# -eq 4 ] || [ $# -eq 5 ]; then
dep=$4
fi
if [ $dep != "nodep" ]; then
    depname="_"$dep
    if [ $signing == "nonsecure" ]; then
    option=$option" -d (0,$dep)"
    fi
    if [ $signing == "secure" ]; then
    option=$option" -d (1,$dep)"
    fi
fi
if [ $# -eq 6 ] || [ $# -eq 5 ] ; then
vername=$5
fi
if [ $vername == "version" ] ; then
ver="_"$version$depname
fi

userAppBinary=$projectdir"/../Binary"
tfm_ns_init=$userAppBinary"/tfm_ns_app_init"$ver".bin"
tfm_s_init=$userAppBinary"/tfm_s_app_init"$ver".bin"
tfm_ns_sign=$userAppBinary"/tfm_ns_app_sign"$ver".bin"
tfm_s_sign=$userAppBinary"/tfm_s_app_sign"$ver".bin"
tfm_s_enc_sign=$userAppBinary"/tfm_s_app_enc_sign"$ver".bin"
tfm_ns_enc_sign=$userAppBinary"/tfm_ns_app_enc_sign"$ver".bin"
# variable for app_image_number = 1
tfm_init=$userAppBinary"/tfm_app_init"$ver".bin"
tfm_enc_sign=$userAppBinary"/tfm_app_enc_sign"$ver".bin"
tfm_sign=$userAppBinary"/tfm_app_sign"$ver".bin"
tfm=$userAppBinary"/tfm_app.bin"

# field updated with tfm_boot postbuild
image_ns_size=
image_s_size=
primary_only=
app_image_number=
crypto_scheme=
external_flash_enable=
ns_code_start=
encrypted=
over_write=
flash_area_scratch_size=
# end of updated field

current_directory=`pwd`
echo $current_directory
cd $projectdir"/../../TFM_SBSFU_Boot/Src"
tfm_key_dir=`pwd`
cd $current_directory
cd $projectdir/../../
envdir=`pwd`
cd $current_directory
#Make sure we have a Binary sub-folder in UserApp folder
if [ ! -e $userAppBinary ]; then
mkdir $userAppBinary
fi
if [ $signing == "secure" ]; then
if [ $app_image_number == 1 ]; then
# when image number is 1, image is assemble and sign during non secure postbuild
exit 0
fi
fi

source $envdir/env.sh

if [ ! -e "$imgtool" ];then
  echo ""
  echo "!!! WARNING : imgtool has not been found on your installation."
  echo ""
  echo "  Install CubeProgrammer on your machine in default path : ~/STMicroelectronics/STM32Cube/STM32CubeProgrammer"
  echo "  or "
  echo "  Update your $envdir/env.sh with the proper path."
  echo ""
  exit 0
fi

#sign mode
if [ $crypto_scheme == 0  ]; then
mode="rsa2048"
fi
if [ $crypto_scheme == 1  ]; then
mode="rsa3072"
fi
if [ $crypto_scheme == 2  ]; then
mode="ec256"
fi
#keys selection
if [ $mode == "rsa2048" ]; then
key_s=$tfm_key_dir"/root-rsa-2048.pem"
key_ns=$tfm_key_dir"/root-rsa-2048_1.pem"
key_enc_pub=$tfm_key_dir"/enc-rsa2048-pub.pem"
fi
if [ $mode == "rsa3072" ]; then
key_s=$tfm_key_dir"/root-rsa-3072.pem"
key_ns=$tfm_key_dir"/root-rsa-3072_1.pem"
key_enc_pub=$tfm_key_dir"/enc-rsa2048-pub.pem"
fi
if [ $mode == "ec256" ]; then
key_s=$tfm_key_dir"/root-ec-p256.pem"
key_ns=$tfm_key_dir"/root-ec-p256_1.pem"
key_enc_pub=$tfm_key_dir"/enc-ec256-pub.pem"
fi
tfm_s=$projectdir"/Secure/Release/TFM_Appli_Secure.bin"
tfm_ns=$projectdir"/NonSecure/Release/TFM_Appli_NonSecure.bin"
#encrypt option is for init image in swap mode
if [ $encrypted == "1" ]; then
encrypt="-E $key_enc_pub -c"
fi
if [ $primary_only == "1" ]; then
option=$option" --primary-only"
encrypt=""
fi

if [ $over_write == "1" ]; then
option=$option" --overwrite-only"
encrypt=""
fi

#swrecord
if [ $app_image_number == "1" ]; then
option=$option" --boot-record=SPE"
elif [ $signing == "nonsecure" ]; then
option=$option" --boot-record=NSPE"
else
option=$option" --boot-record=SPE"
fi

#signing
echo $signing" "$mode" "$option" app_image_number="$app_image_number

if [ $app_image_number == 1 ]; then
echo assemble image
command_ass=" ass -f "$tfm_s" -o "$image_s_size" -i 0x0 "$tfm_ns" "$tfm
echo "$imgtool" $command_ass
"$imgtool" $command_ass >> $projectdir"/output.txt"
ret=$?
if [ $ret != 0 ]; then
echo "postbuild.sh failed"
exit 1
fi
# sign assemble binary as secure image, slot size is image_ns_size
let "image_s_size=$image_ns_size"
signing=
tfm_s=$tfm
tfm_s_init=$tfm_init
tfm_s_sign=$tfm_sign
tfm_s_enc_sign=$tfm_enc_sign
fi

#nb sectors in image areas
if [ $over_write == "1" ]; then
    image_ns_sectors=""
    image_s_sectors=""
    flag=""
else
    let image_ns_sectors="((($image_ns_size+1) / $flash_area_scratch_size)+1)"
    let image_s_sectors="((($image_s_size+1) / $flash_area_scratch_size)+1)"
    flag="-M"
fi

echo $signing signing
if [ "$signing" == "nonsecure" ]; then
command_init=" sign -k "$key_ns" "$encrypt" -S "$image_ns_size" "$flag" "$image_ns_sectors" "$option" -v "$version" --confirm --pad "$tfm_ns" "$tfm_ns_init
else
command_init=" sign -k "$key_s" "$encrypt" -S "$image_s_size" "$flag" "$image_s_sectors" "$option" -v "$version" --confirm --pad "$tfm_s" "$tfm_s_init
fi
"$imgtool" $command_init >> $projectdir"/output.txt"
ret=$?
if [ $ret != 0 ]; then
echo "postbuild.sh failed"
exit 1
fi
if [ "$signing" == "nonsecure" ]; then
command_sign=" sign -k "$key_ns" -S "$image_ns_size" "$flag" "$image_ns_sectors" "$option" -v "$version" "$tfm_ns" "$tfm_ns_sign
else
command_sign=" sign -k "$key_s" -S "$image_s_size" "$flag" "$image_s_sectors" "$option" -v "$version" "$tfm_s" "$tfm_s_sign
fi
"$imgtool" $command_sign >> $projectdir"/output.txt"
ret=$?
if [ $ret != 0 ]; then
echo "postbuild.sh failed"
exit 1
fi
echo $signing encrypting
if [ "$signing" == "nonsecure" ]; then
command_enc=" sign -k "$key_ns" -E "$key_enc_pub" -S "$image_ns_size" "$flag" "$image_ns_sectors" "$option" -v "$version" "$tfm_ns" "$tfm_ns_enc_sign
else
command_enc=" sign -k "$key_s" -E "$key_enc_pub" -S "$image_s_size" "$flag" "$image_s_sectors" "$option" -v "$version" "$tfm_s" "$tfm_s_enc_sign
fi
"$imgtool" $command_enc >> $projectdir"/output.txt"
ret=$?
if [ $ret != 0 ]; then
echo "postbuild.sh failed"
exit 1
fi
exit 0
