## <b>OpenBootloader Application Description</b>

This application exploits OpenBootloader Middleware to demonstrate how to develop an IAP application
and how use it.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 60MHz.

Then, these protocols and memories interfaces are configured:

  - USART2
  - I2C3
  - SPI3
  - FLASH interface
  - RAM interface
  - Option bytes interface
  - System memory interface
  - OTP interface
  - Engineering bytes interface

Then, the OpenBootloader application will wait for incoming communication on one of the supported protocols.

Once a communication is established through one of the supported protocols,
the OpenBootloader application will wait for commands sent by the host.

#### <b>Notes</b>

 1. In case of Mass Erase operation, the OpenBootloader FLASH area must be protected otherwise the OpenBootloader
    will be erased.

 2. In the `OpenBootloader_Init()` function in `app_openbootloader.c` file, the user can:
       - Select the list of supported commands for a specific interface by defining its own list of commands.

       Here is an example of how to customize USART interface commands list, here only read/write commands are supported:

        OPENBL_CommandsTypeDef USART_Cmd =
        {
          NULL,
          NULL,
          NULL,
          OPENBL_USART_ReadMemory,
          OPENBL_USART_WriteMemory,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL,
          NULL
        };

        USART_Handle.p_Ops = &USART_Ops;
        USART_Handle.p_Cmd = &USART_Cmd;           /* Initialize the USART handle with the list of supported commands */
        OPENBL_USART_SetCommandsList(&USART_Cmd);  /* Register the list of supported commands in MW side */

       - Use the default list of supported commands for a specific interface by reusing the commands list defined in MW side.

       Here is an example of how to use USART interface default commands list:

        /* Register USART interfaces */
        USART_Handle.p_Ops = &USART_Ops;
        USART_Handle.p_Cmd = OPENBL_USART_GetCommandsList();  /* Initialize the USART handle with the default list supported commands */

### <b>Keywords</b>

OpenBootloader, USART, I2C, SPI

### <b>Directory contents</b>

     - Core/Src/main.c                                    Main program file
     - Core/Src/stm32wbaxx_hal_msp.c                      MSP Initialization file
     - Core/Src/stm32wbaxx_it.c                           Interrupt handlers file
     - Core/Src/system_stm32wbaxx.c                       STM32WBAxx system clock configuration file
     - Core/Inc/main.h                                    Main program header file
     - Core/Inc/stm32wbaxx_hal_conf.h                     HAL Library Configuration file
     - Core/Inc/stm32wbaxx_it.h                           Interrupt handlers header file
     - OpenBootloader/App/app_openbootloader.c            OpenBootloader application entry point
     - OpenBootloader/App/app_openbootloader.h            Header for OpenBootloader application entry file
     - OpenBootloader/Target/common_interface.c           Contains common functions used by different interfaces
     - OpenBootloader/Target/common_interface.h           Header for common functions file
     - OpenBootloader/Target/engibytes_interface.c        Contains Engibytes interface
     - OpenBootloader/Target/engibytes_interface.h        Header for Engibytes functions file
     - OpenBootloader/Target/flash_interface.c            Contains FLASH interface
     - OpenBootloader/Target/flash_interface.h            Header of FLASH interface file
     - OpenBootloader/Target/i2c_interface.c              Contains I2C interface
     - OpenBootloader/Target/i2c_interface.h              Header of I2C interface file
     - OpenBootloader/Target/iwdg_interface.c             Contains IWDG interface
     - OpenBootloader/Target/iwdg_interface.h             Header of IWDG interface file
     - OpenBootloader/Target/optionbytes_interface.c      Contains OptionBytes interface
     - OpenBootloader/Target/optionbytes_interface.h      Header of OptionBytes interface file
     - OpenBootloader/Target/openbootloader_conf.h        Header file that contains OpenBootloader HW dependent configuration
     - OpenBootloader/Target/otp_interface.c              Contains OTP interface
     - OpenBootloader/Target/otp_interface.h              Header of OTP interface file
     - OpenBootloader/Target/ram_interface.c              Contains RAM interface
     - OpenBootloader/Target/ram_interface.h              Header of RAM interface file
     - OpenBootloader/Target/spi_interface.c              Contains SPI interface
     - OpenBootloader/Target/spi_interface.h              Header of SPI interface file
     - OpenBootloader/Target/systemmemory_interface.c     Contains ICP interface
     - OpenBootloader/Target/systemmemory_interface.h     Header of ICP interface file
     - OpenBootloader/Target/usart_interface.c            Contains USART interface
     - OpenBootloader/Target/usart_interface.h            Header of USART interface file

### <b>Hardware and Software environment</b>

  - This application runs on STM32WBA52xx devices.

  - This example has been tested with a NUCLEO-WBA52CG board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-WBA52CG set-up to use USART:
    - To use the USART2 for communication you have to connect:
      - Tx pin of your host adapter to PA12 pin (CN4 17) GPIO37
      - Rx pin of your host adapter to PA11 pin (CN4 24) GPIO45

  - NUCLEO-WBA52CG set-up to use I2C
    - Set I2C address to 0x66
    - To use the I2C3 for communication you have to connect:
      - SCL pin of your host adapter to PA6 pin (CN3 30) GPIO12
      - SDA pin of your host adapter to PA7 pin (CN3 28) GPIO11

  - NUCLEO-WBA52CG set-up to use SPI
    - To use the SPI3 for communication you have to connect:
      - SCK  pin of your host adapter to PA0 pin (CN3 38) GPIO22
      - MISO pin of your host adapter to PB9 pin (CN4 23) GPIO42
      - MOSI pin of your host adapter to PB8 pin (CN4 38) GPIO57
      - NSS pin of your host adapter to PA5 pin (CN3 36) GPIO21
        This is optional as NSS pin is configured in SW mode, so the user can leave it unwired

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

  - Open your preferred toolchain using the adequate version
  - Rebuild all files and load your image into target memory
  - Run the application
  - Run STM32CubeProgrammer and 
  - Connect to OpenBootloader using USART2
    Or
  - Connect your I2C host adapter and connect to OpenBootloader using I2C3
    Or
  - Connect your SPI host adapter and connect to OpenBootloader using SPI3
