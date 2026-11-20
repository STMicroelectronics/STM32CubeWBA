## <b>ROT_Provisioning OEMiRoT_TFM Description</b>

This section provides an overview of the available scripts for OEMiRoT_TFM boot path.

OEMiROT stands for "OEM Immutable Root of Trust" and it provides two services:

  - Secure Boot: Verification of the integrity, and authenticity of the application code before any execution.
  - Secure Firmware Update: If a new firmware image is available on detection, check of its authenticity.
                            Once the firmware is decrypted, check of the integrity of the code before installing it.

### <b>Keywords</b>

OEMiROT, boot path, Root Of Trust, Security, mcuboot

### <b>Directory contents</b>

<b>Sub-directories</b>

- OEMiRoT_TFM/Binary                               Output binaries and generated images.
- OEMiRoT_TFM/Config                               OEMiROT configuration files.
- OEMiRoT_TFM/Images                               Image configuration files.
- OEMiRoT_TFM/Keys                                 Keys for firmware image authentication and encryption.

<b>Scripts</b>

- OEMiRoT_TFM/img_config.bat/.sh                   Performs configuration of data and images.
- OEMiRoT_TFM/ob_flash_programming.bat/.sh         Programs option bytes and firmware image on the device.
- OEMiRoT_TFM/provisioning.bat/.sh                 Performs device provisioning process.
- OEMiRoT_TFM/regression.bat/.sh                   Performs full regression of the device.

### <b>Hardware and Software environment</b>

- This example has been tested with STMicroelectronics STM32WBA65I-DK1 (MB2130)
  board and can be easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

To use OEMiRoT_TFM bootpath, you should first configure ROT_Provisioning/env.bat/.sh script
(tools path, application path and COM port configuration).<br>
The .bat scripts are designed for Windows, whereas the .sh scripts are designed for Linux and Mac-OS.

Then you should run the provisioning script (provisioning.bat/.sh).<br>
During the **provisioning process**, the programming scripts and the application files will
be automatically updated according to OEMiRoT_TFM configuration, and user answers.

The **provisioning process** (OEMiRoT_TFM/provisioning.bat/.sh) is divided into 3 majors steps:

- Step 1: Configuration management
- Step 2: Images generation
- Step 3: Provisioning

The provisioning script is relying on ob_flash_programming and img_config scripts.

### <b>Notes</b>

**AppliCfg.py** (located in Utilities/PC_Software/ROT_AppliConfig) is used during provisioning
process. It is needed to have python and some python packages installed in your
environment. Refer to Utilities/PC_Software/ROT_AppliConfig/README.md for more details.
