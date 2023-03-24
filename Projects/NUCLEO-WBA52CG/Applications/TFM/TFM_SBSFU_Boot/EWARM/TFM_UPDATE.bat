echo TFM_UPDATE started
set stm32programmercli="C:\Program Files\STMicroelectronics\STM32Cube\STM32CubeProgrammer\bin\STM32_Programmer_CLI.exe"
set connect_no_reset=-c port=SWD mode=UR ap=1
set connect=-c port=SWD mode=UR ap=1 --hardRst
set slot0=
set slot1=
set slot2=
set slot3=
set slot4=
set slot5=
set slot6=
set slot7=
set boot=
set loader=
set cfg_loader=
set app_image_number=
set s_data_image_number=
set ns_data_image_number=
if  "%app_image_number%" == "2" (
echo Write TFM_Appli Secure
%stm32programmercli% %connect% -d %~dp0..\..\TFM_Appli\binary\tfm_s_app_init.bin %slot0% -v
IF %errorlevel% NEQ 0 goto :error
echo TFM_Appli Secure Written
echo Write TFM_Appli NonSecure
%stm32programmercli% %connect% -d %~dp0..\..\TFM_Appli\binary\tfm_ns_app_init.bin %slot1% -v
IF %errorlevel% NEQ 0 goto :error
echo TFM_Appli NonSecure Written
)
if  "%app_image_number%" == "1" (
echo Write TFM_Appli
%stm32programmercli% %connect% -d %~dp0..\..\TFM_Appli\binary\tfm_app_init.bin %slot0% -v
IF %errorlevel% NEQ 0 goto :error
echo "TFM_Appli Written"
)
if  "%s_data_image_number%" == "1" (
echo Write TFM_Appli Secure Data
IF not exist %~dp0..\..\TFM_Appli\binary\tfm_s_data_init.bin (
@echo [31mError: tfm_s_data_init.bin does not exist! Run dataimg.bat script to generate it[0m
goto :error
)
%stm32programmercli% %connect% -d %~dp0..\..\TFM_Appli\binary\tfm_s_data_init.bin %slot4% -v
IF %errorlevel% NEQ 0 goto :error
echo TFM_Appli Secure Data Written
)
if  "%ns_data_image_number%" == "1" (
echo Write TFM_Appli NonSecure Data
IF not exist %~dp0..\..\TFM_Appli\binary\tfm_ns_data_init.bin (
@echo [31mError: tfm_ns_data_init.bin does not exist! Run dataimg.bat script to generate it[0m
goto :error
)
%stm32programmercli% %connect% -d %~dp0..\..\TFM_Appli\binary\tfm_ns_data_init.bin %slot5% -v
IF %errorlevel% NEQ 0 goto :error
echo TFM_Appli NonSecure Data Written
)
:: write loader if config loader is active
if  "%cfg_loader%" == "1" (
echo Write TFM_Loader
%stm32programmercli% %connect% -d %~dp0..\..\TFM_Loader\binary\loader.bin %loader% -v
IF %errorlevel% NEQ 0 goto :error
echo TFM_Loader  Written
)
echo Write TFM_SBSFU_Boot
%stm32programmercli% %connect% -d %~dp0.\NUCLEO-WBA52CG\Exe\project.bin %boot% -v
IF %errorlevel% NEQ 0 goto :error
echo TFM_SBSFU_Boot Written
echo TFM_UPDATE script done, press a key
IF [%1] NEQ [AUTO] pause
exit 0

:error
echo TFM_UPDATE script failed, press key
IF [%1] NEQ [AUTO] pause
exit 1