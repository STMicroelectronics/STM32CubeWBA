## <b>ROT Application Description</b>

The STM32WBA55G-DK1 ROT project is based on a recursive copy of STM32WBA65I-DK1/Applications/ROT/ content and few changes.

### <b>How to use it ?</b>

Refer to the following wiki page to start ROT on WBA55.

  - [STM32WBA security](https://wiki.st.com/stm32mcu/wiki/Category:STM32WBA).
  - [How to create RoT examples for STM32WBA55](https://wiki.st.com/stm32mcu/wiki/Security:How_to_create_RoT_examples_for_STM32WBA55).

Refer to STM32WBA65I-DK1/Applications/ROT wiki page for more information.

#### <b>Notes:</b>

By default the anti-tamper is enabled for internal tamper events only. It is possible to change this configuration with
OEMIROT_TAMPER_ENABLE define in Inc\\boot_hal_cfg.h.

```
#define NO_TAMPER            (0)                   /*!< No tamper activated */
#define INTERNAL_TAMPER_ONLY (1)                   /*!< Only Internal tamper activated */
#define ALL_TAMPER           (2)                   /*!< Internal and External tamper activated */
#define OEMIROT_TAMPER_ENABLE INTERNAL_TAMPER_ONLY /*!< TAMPER configuration flag  */
```

If OEMIROT_TAMPER_ENABLE is changed to ALL_TAMPER, the anti-tamper protection is enabled with passive tamper pin usage.
It is needed to connect TAMP_IN4 (PC13 on CN10 pin 5) to VCC on the STM32WBA55G-DK1 board and reset the board to trigger a tamper event.
When nothing is connected to TAMP_IN4, the application is running.
Moreover, no warning message about anti-tamper detection will be displayed if OEMIROT_DEV_MODE enabled.
