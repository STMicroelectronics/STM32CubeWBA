#!/bin/bash -
# arg1 is the build directory
project_loader_dir=$1
# arg2 is operation
operation=$2
# Getting the Trusted Package Creator CLI path
SCRIPT=$(readlink -f $0)
project_dir=`dirname $SCRIPT`
cd "$project_dir/../../../../ROT_Provisioning"
provisioningdir=$(pwd)
cd $project_dir
source $provisioningdir/env.sh "$provisioningdir"

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

secure_nsc=$project_loader_dir"/../Secure_nsclib/secure_nsclib.o"
maxbytesize=350
loader_s=$project_loader_dir"/Secure/Release/loader_s.bin"
loader_ns=$project_loader_dir"/NonSecure/Release/loader_ns.bin"
binarydir=$project_loader_dir"/../Binary"
loader=$binarydir"/loader.bin"
loader_ns_size=0x6000
loader_s_size=0x4000

#Make sure we have a Binary sub-folder in UserApp folder
if [ ! -e $binarydir ]; then
echo "create Binary dir"
mkdir $binarydir
fi

if [ $operation == "secure" ]; then
ret=0
elif [ $operation == "nonsecure" ]; then
#according to secure_nsclib.o size select build with or without MCUBOOT_PRIMARY_ONLY
filesize=$(stat -c%s "$secure_nsc")

if [ $filesize -ge $maxbytesize ]; then
echo "loader with secure part (MCUBOOT_PRIMARY_ONLY defined)" >> $project_loader_dir/postbuild.log
"$imgtool" ass -f "$loader_s" -o "$loader_s_size" -i "$loader_ns_size" "$loader_ns" "$loader" >> $project_loader_dir"/postbuild.log"
ret=$?
else
#loader without secure part
echo "loader without secure part (MCUBOOT_PRIMARY_ONLY not defined)" >> $project_loader_dir/postbuild.log
"$imgtool" ass  -i "$loader_ns_size" "$loader_ns" "$loader" >> $project_loader_dir"/postbuild.log"
ret=$?
fi
fi
if [ $ret == 0 ]; then
echo "postbuild.sh done" $operation

#end of generate version for anti roll back test
else
echo "postbuild.sh failed"
fi
