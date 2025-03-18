## <b>ROT_Provisioning OEMiROT_OEMuROT Description</b>

This section provides an overview of the available scripts for OEMiROT_OEMuROT boot path.

OEMiROT stands for "OEM Immutable Root of Trust".<br>
OEMuROT stands for "OEM Updatable Root of Trust".<br>
The 2 boot stages provides two services:

  - Secure Boot: Verification of the integrity, and authenticity of the application code before any execution.
  - Secure Firmware Update: If a new firmware image is available on detection, check of its authenticity.
                            Once the firmware is decrypted, check of the integrity of the code before installing it.

### <b>Keywords</b>

OEMiROT, OEMuROT, boot path, Root Of Trust, Security, mcuboot

### <b>Directory contents</b>

<b>Sub-directories</b>

- OEMiROT_OEMuROT/Binary                               Output binaries and generated images.
- OEMiROT_OEMuROT/Config                               OEMiROT and OEMuROT configuration files.
- OEMiROT_OEMuROT/Image                                Image configuration files.
- OEMiROT_OEMuROT/Keys                                 Keys for firmware image authentication and encryption.

<b>Scripts</b>

- OEMiROT_OEMuROT/img_config.bat/.sh                   Performs configuration of data and images.
- OEMiROT_OEMuROT/ob_flash_programming.bat/.sh         Programs option bytes and firmware image on the device.
- OEMiROT_OEMuROT/provisioning.bat/.sh                 Performs device provisioning process.
- OEMiROT_OEMuROT/regression.bat/.sh                   Performs full regression of the device.

### <b>Hardware and Software environment</b>

- This example has been tested with STMicroelectronics STM32WBA65I-DK1 (MB2130)
  board and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

To use OEMiROT_OEMuROT bootpath, you should first configure ROT_Provisioning/env.bat/.sh script
(tools path, application path and COM port configuration).<br>
The .bat scripts are designed for Windows, whereas the .sh scripts are designed for Linux and Mac-OS.

Then you should run the provisioning script (provisioning.bat/.sh).<br>
During the **provisioning process**, the programming scripts, the OEMuRoT and the application files will
be automatically updated according to OEMiROT_OEMuROT configuration, and user answers.

The **provisioning process** (OEMiROT_OEMuROT/provisioning.bat/.sh) is divided into 3 majors steps:

- Step 1: Configuration management
- Step 2: Images generations
- Step 3: Provisioning

The provisioning script is relying on ob_flash_programming and img_config scripts.

### <b>Notes</b>

Two versions of AppliCfg (used during **provisioning process**) are available: windows executable and python version.<br>
On Windows environment, the executable is used. On other environment, the python version is automatically used.<br>
To use Python version, it is needed to:

- have python installed (Python 3.6 or newer) and available in execution path variable
- have required python modules installed (Utilities/PC_Software/ROT_AppliConfig/requirements.txt):

        pip install -r requirements.txt
