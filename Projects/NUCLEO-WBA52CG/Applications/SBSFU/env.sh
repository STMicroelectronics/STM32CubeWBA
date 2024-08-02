#!/bin/bash

# ==============================================================================
#                               General
# ==============================================================================
#Configure tools installation path
if [ "$OS" == "Windows_NT" ]; then
  imgtool="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\Utilities\Windows\imgtool.exe"
  stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
  stm32tpccli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32TrustedPackageCreator_CLI.exe"
else
  imgtool="~/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/Utilities/Linux/imgtool"
  stm32programmercli="~/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI"
  stm32tpccli="~/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32TrustedPackageCreator_CLI"
fi