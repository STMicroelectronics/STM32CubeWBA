## <b>ROT_Provisioning OEMiROT Description</b>

This section provides an overview of the available scripts for OEMiROT boot path.

OEMiROT stands for "OEM Immutable Root of Trust" and it provides two services:

  - Secure Boot: Verification of the integrity, and authenticity of the application code before any execution.
  - Secure Firmware Update: If a new firmware image is available on detection, check of its authenticity.
                            Once the firmware is decrypted, check of the integrity of the code before installing it.

### <b>Keywords</b>

OEMiROT, boot path, Root Of Trust, Security, mcuboot

### <b>Directory contents</b>

<b>Sub-directories</b>

- OEMiROT/Binary                               Output binaries and generated images.
- OEMiROT/Config                               OEMiROT configuration files.
- OEMiROT/Images                               Image configuration files.
- OEMiROT/Keys                                 Keys for firmware image authentication and encryption.

<b>Scripts</b>

- OEMiROT/img_config.bat/.sh                   Performs configuration of data and images.
- OEMiROT/ob_flash_programming.bat/.sh         Programs option bytes and firmware image on the device.
- OEMiROT/provisioning.bat/.sh                 Performs device provisioning process.
- OEMiROT/regression.bat/.sh                   Performs full regression of the device.

### <b>Hardware and Software environment</b>

- This example has been tested with STMicroelectronics NUCLEO-WBA25CE (MB2293)
  board and can be easily tailored to any other supported device and development board.


### <b>How to use it ?</b>

To use OEMiRoT bootpath, you should first configure ROT_Provisioning/env.bat/.sh script
(tools path, application path and COM port configuration).<br>
The .bat scripts are designed for Windows, whereas the .sh scripts are designed for Linux and Mac-OS.

Then you should run the provisioning script (provisioning.bat/.sh).<br>
During the **provisioning process**, the programming scripts and the application files will
be automatically updated according to OEMiRoT configuration, and user answers.

The **provisioning process** (OEMiROT/provisioning.bat/.sh) is divided into 3 majors steps:

- Step 1: Configuration management
- Step 2: Images generation
- Step 3: Provisioning

The provisioning script is relying on ob_flash_programming and img_config scripts.

### <b>Notes</b>

**AppliCfg.py** (located in Utilities/PC_Software/ROT_AppliConfig) is used during provisioning
process. It is needed to have python and some python packages installed in your
environment. Refer to Utilities/PC_Software/ROT_AppliConfig/README.md for more details.
