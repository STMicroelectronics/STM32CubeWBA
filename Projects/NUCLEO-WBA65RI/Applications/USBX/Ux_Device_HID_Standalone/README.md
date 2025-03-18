## <b>Ux_Device_HID_Standalone Application Description </b>

This application provides an example of USBX stack usage on NUCLEO-WBA65RI board,
It shows how to develop USB Device Human Interface "HID" mouse based bare metal application.

The application is designed to emulate an USB HID mouse device, the code provides all required device descriptors framework
and associated Class descriptor report to build a compliant USB HID mouse device.

The application's main calls the MX_USBX_Device_Init() function in order to Initialize USBX and USBX_Device_Process in the while loop.

As stated earlier, the present application runs in standalone mode without ThreadX, for this reason, the standalone variant of USBX is enabled by adding the following flag in ux_user.h:

 - #define UX_STANDALONE

To customize the HID application by sending the mouse position step by step every 10ms.
For each 10ms, the application calls the GetPointerData() API to update the mouse position (x, y) and send
the report buffer through the ux_device_class_hid_event_set() API.

#### <b>Expected success behavior</b>

When plugged to PC host, the NUCLEO-WBA65RI must be properly enumerated as an USB HID mouse device.
During the enumeration phase, the device must provide host with the requested descriptors (Device descriptor, configuration descriptor, string descriptors).
Those descriptors are used by host driver to identify the device capabilities. Once the NUCLEO-WBA65RI USB device successfully completed the enumeration phase, the device sends a HID report after a user button press.
Each report sent should move the mouse cursor by one step on host side.

#### <b>Error behaviors</b>

Host PC shows that USB device does not operate as designed (Mouse enumeration failed, the mouse pointer doesn't move).

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" Specification and HID class Specification.

#### <b> Known limitations</b>

None

### <b>Keywords</b>

Standalone, USBXDevice, USB_OTG, high Speed, HID, Mouse

### <b>Hardware and Software environment</b>
  - This application runs on STM32WBAxx devices.
  - This application has been tested with STMicroelectronics NUCLEO-WBA65RI boards revision MB2130-WBA65RI-A01 and can be easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application