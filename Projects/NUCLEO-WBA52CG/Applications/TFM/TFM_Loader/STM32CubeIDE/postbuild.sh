#!/bin/bash -
# arg1 is the build directory
# arg2 is operation
projectdir=$1
operation=$2
secure_nsc=$projectdir"/../../../Secure_nsclib/secure_nsclib.o"
maxbytesize=350
loader_s=$projectdir"/../../Secure/Release/TFM_Loader_Secure.bin"
binarydir=$projectdir"/../../../Binary"
loader_ns=$projectdir"/TFM_Loader_NonSecure.bin"
loader=$binarydir"/loader.bin"
loader_ns_size=
loader_s_size=
current_directory=`pwd`
echo $current_directory
cd $projectdir/../../
envdir=`pwd`
cd $current_directory
#Make sure we have a Binary sub-folder in UserApp folder
if [ ! -e $binarydir ]; then
echo "create Binary dir"
mkdir $binarydir
fi

source $envdir/../../env.sh

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

if [ $operation == "secure" ]; then
ret=0
elif [ $operation == "nonsecure" ]; then
#according to secure_nsclib.o size select build with or without MCUBOOT_PRIMARY_ONLY
filesize=$(stat -c%s "$secure_nsc")

if [ $filesize -ge $maxbytesize ]; then
echo "loader with secure part (MCUBOOT_PRIMARY_ONLY defined)" >> $projectdir/output.txt
command=" ass -f "$loader_s" -o "$loader_s_size" -i "$loader_ns_size" "$loader_ns" "$loader
"$imgtool" $command  >> $projectdir"/output.txt"
ret=$?
else
#loader without secure part
echo "loader without secure part (MCUBOOT_PRIMARY_ONLY not defined)" >> $projectdir/output.txt
command=" ass  -i "$loader_ns_size" "$loader_ns" "$loader
"$imgtool" $command  >> $projectdir"/output.txt"
ret=$?
fi
fi
if [ $ret == 0 ]; then
echo "postbuild.sh done" $operation

#end of generate version for anti roll back test
else
echo "postbuild.sh failed"
fi
