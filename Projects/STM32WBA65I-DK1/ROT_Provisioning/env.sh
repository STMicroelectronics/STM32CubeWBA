#!/bin/bash -
# Absolute path to this script
if [ $# -ge 1 ] && [ -d $1 ]; then
    projectdir=$1
else
    projectdir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
fi

# ==============================================================================
#                            General
# ==============================================================================
# Configure tools installation path
if [ "$OS" == "Windows_NT" ]; then
    stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
    stm32tpccli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32TrustedPackageCreator_CLI.exe"
    imgtool="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\Utilities\Windows\imgtool.exe"
else
    stm32programmercli_path=~/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/
    PATH=$stm32programmercli_path:$PATH
    PATH=$stm32programmercli_path/Utilities/Linux/:$PATH
    stm32programmercli="STM32_Programmer_CLI"
    stm32tpccli="STM32TrustedPackageCreator_CLI"
    imgtool="imgtool"
fi

# ==============================================================================
#                            OEMiROT Appli path
# ==============================================================================
# Select Application project below
# oemirot_appli_path_project="Templates/ROT/OEMiROT_Appli"
# oemirot_appli_path_project="Applications/ROT/OEMiROT_Appli"
# oemirot_appli_path_project="Templates/ROT/OEMiROT_Appli_TrustZone"
oemirot_appli_path_project="Applications/ROT/OEMiROT_Appli_TrustZone"

# ==============================================================================
#                            OEMiROT Loader path
# ==============================================================================
oemirot_loader_trustzone_path_project="Applications/ROT/OEMiROT_Loader_TrustZone"

# ==============================================================================
#                            OEMiROT Boot path
# ==============================================================================
# Select OEMiROT Boot project below
oemirot_boot_path_project="Applications/ROT/OEMiROT_Boot"

# ==============================================================================
#                         OEMiRoT_OEMuRoT boot path
# ==============================================================================
# Select oemirot binary
oemirot_boot_path_binary=/ROT_Provisioning/OEMiRoT_OEMuRoT/Binary/OEMiROT_Boot.bin
# oemurot project and application project are the same as for OEMiROT boot path

# ==============================================================================

cube_fw_path=$projectdir/../../../
rot_provisioning_path=$projectdir
oemirot_appli_secure=oemirot_tz_s_app_init_sign.bin
oemirot_appli_non_secure=oemirot_tz_ns_app_init_sign.bin
oemirot_appli_assembly_sign=oemirot_tz_app_init_sign.bin
oemurot_appli_secure=oemurot_tz_s_app_init_sign.bin
oemurot_appli_non_secure=oemurot_tz_ns_app_init_sign.bin
oemurot_appli_assembly_sign=oemurot_tz_app_init_sign.bin
