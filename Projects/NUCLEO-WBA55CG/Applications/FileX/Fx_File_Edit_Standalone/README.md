
## <b>Fx_File_Edit_Standalone Application Description</b>

This application provides an example of FileX stack usage on NUCLEO-WBA55CG board, running in standalone mode (without ThreadX).

It demonstrates how to create a Fat File system on the internal SRAM memory using FileX.

The application is designed to execute file operations on the SRAM-Disk device, it provides all required software code for properly managing it.

The application's main calls the MX_FileX_Init() function that handles file operations. At this stage, all FileX resources are created and the SRAM
driver is initialized. After that, the MX_FileX_Process will start by formatting the SRAM-Disk using FileX services. The resulting file system is a FAT32
compatible, with 512 bytes per sector.

Upon successful opening of the created SRAM-Disk media, FileX continue with creating a file called "STM32.TXT" into the root directory, then write into
it some predefined data. Then file is re-opened in read only mode and content is checked.

As stated earlier, the present application runs in standalone mode without ThreadX, for this reason, the standalone variant of fileX is used, plus the following
flags need to be set in fx_user.h:

  #define FX_SINGLE_THREAD

  #define FX_STANDALONE_ENABLE

#### <b>Expected success behavior</b>

Successful operation is marked by a toggling green LED light.

Also, information regarding executing operation on the SRAM-Disk is printed to the serial port.

#### <b>Error behaviors</b>

 - On failure, the red LED starts toggling while the green LED is switched OFF.

#### <b>Assumptions if any</b>
None

#### <b>Known limitations</b>
None

### <b>Notes</b>

 1. The created SRAM-Disk, is placed in SRAM2(64 KB) starting from the(SRAM2_BASE=@0x20010000).
 2. First 512 byte of this memory is reserved for FAT file system, the rest is available for file storage.

#### <b>FileX/LevelX usage hints</b>

- When calling the fx_media_format() API, it is highly recommended to understand all the parameters used by the API to correctly generate a valid filesystem.

### <b>Keywords</b>

FileX, File System, FAT32, SRAM, SRAM-DISK

### <b>Hardware and Software environment</b>

  - This application runs on STM32WBAxx devices.
  - This application has been tested with STMicroelectronics NUCLEO-WBA55CG boards revision MB1803-WBA55CGA-B01
    and can be easily tailored to any other supported device and development board.

  - This application uses USART1 to display logs, the hyperterminal configuration is as follows:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - Stop Bit = 1
      - Parity = None
      - Flow control = None


### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
