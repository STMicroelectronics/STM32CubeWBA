;/*
; * Copyright (c) 2009-2021 Arm Limited
; *
; * Licensed under the Apache License, Version 2.0 (the "License");
; * you may not use this file except in compliance with the License.
; * You may obtain a copy of the License at
; *
; *     http://www.apache.org/licenses/LICENSE-2.0
; *
; * Unless required by applicable law or agreed to in writing, software
; * distributed under the License is distributed on an "AS IS" BASIS,
; * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; * See the License for the specific language governing permissions and
; * limitations under the License.
; *
; *
; * This file is derivative of CMSIS V5.00 gcc_arm.ld
; */

/*********** WARNING: This is an auto-generated file. Do not edit! ***********/

/* Linker script to configure memory regions. */
/* This file will be run through the pre-processor. */

#include "region_defs.h"
#define TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
#define TFM_PARTITION_PROTECTED_STORAGE
#define TFM_PARTITION_CRYPTO
#define TFM_PARTITION_PLATFORM
#define TFM_PARTITION_INITIAL_ATTESTATION
MEMORY
{
  FLASH    (rx)  : ORIGIN = S_CODE_START, LENGTH = S_CODE_SIZE
  RAM      (rwx) : ORIGIN = S_DATA_START + BOOT_SHARED_DATA_SIZE , LENGTH = S_DATA_SIZE - BOOT_SHARED_DATA_SIZE
  VENEERS  (rx)  : ORIGIN = CMSE_VENEER_REGION_START, LENGTH = CMSE_VENEER_REGION_SIZE
}

__heap_size__  = S_HEAP_SIZE;
__psp_stack_size__ = S_PSP_STACK_SIZE;
__msp_init_stack_size__ = S_MSP_STACK_SIZE_INIT;

/* Library configurations */
GROUP(libgcc.a libc.a libm.a libnosys.a libc_nano.a)

ENTRY(Reset_Handler)

SECTIONS
{
    .TFM_VECTORS : ALIGN(4)
    {
        __vectors_start__ = .;
        KEEP(*(.vectors))
        __vectors_end__ = .;
        *startup*(.text*)
        . = ALIGN(4);
    } > FLASH

    .copy.table : ALIGN(4)
    {
        __copy_table_start__ = .;
        LONG (LOADADDR(.TFM_DATA))
        LONG (ADDR(.TFM_DATA))
        LONG (SIZEOF(.TFM_DATA)/4)
        LONG (LOADADDR(.TFM_UNPRIV_DATA))
        LONG (ADDR(.TFM_UNPRIV_DATA))
        LONG (SIZEOF(.TFM_UNPRIV_DATA)/4)
#ifdef TFM_PARTITION_PROTECTED_STORAGE
        LONG (LOADADDR(.ER_TFM_SP_PS_DATA))
        LONG (ADDR(.ER_TFM_SP_PS_DATA))
        LONG (SIZEOF(.ER_TFM_SP_PS_DATA)/4)
#endif /* TFM_PARTITION_PROTECTED_STORAGE */
#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
        LONG (LOADADDR(.ER_TFM_SP_ITS_DATA))
        LONG (ADDR(.ER_TFM_SP_ITS_DATA))
        LONG (SIZEOF(.ER_TFM_SP_ITS_DATA)/4)
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */
#ifdef TFM_PARTITION_CRYPTO
        LONG (LOADADDR(.ER_TFM_SP_CRYPTO_DATA))
        LONG (ADDR(.ER_TFM_SP_CRYPTO_DATA))
        LONG (SIZEOF(.ER_TFM_SP_CRYPTO_DATA)/4)
#endif /* TFM_PARTITION_CRYPTO */
#ifdef TFM_PARTITION_PLATFORM
        LONG (LOADADDR(.ER_TFM_SP_PLATFORM_DATA))
        LONG (ADDR(.ER_TFM_SP_PLATFORM_DATA))
        LONG (SIZEOF(.ER_TFM_SP_PLATFORM_DATA)/4)
#endif /* TFM_PARTITION_PLATFORM */
#ifdef TFM_PARTITION_INITIAL_ATTESTATION
        LONG (LOADADDR(.ER_TFM_SP_INITIAL_ATTESTATION_DATA))
        LONG (ADDR(.ER_TFM_SP_INITIAL_ATTESTATION_DATA))
        LONG (SIZEOF(.ER_TFM_SP_INITIAL_ATTESTATION_DATA)/4)
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */
#ifdef TFM_PARTITION_TEST_CORE
        LONG (LOADADDR(.ER_TFM_SP_CORE_TEST_DATA))
        LONG (ADDR(.ER_TFM_SP_CORE_TEST_DATA))
        LONG (SIZEOF(.ER_TFM_SP_CORE_TEST_DATA)/4)
#endif /* TFM_PARTITION_TEST_CORE */
#ifdef TFM_PARTITION_TEST_CORE
        LONG (LOADADDR(.ER_TFM_SP_CORE_TEST_2_DATA))
        LONG (ADDR(.ER_TFM_SP_CORE_TEST_2_DATA))
        LONG (SIZEOF(.ER_TFM_SP_CORE_TEST_2_DATA)/4)
#endif /* TFM_PARTITION_TEST_CORE */
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
        LONG (LOADADDR(.ER_TFM_SP_SECURE_TEST_PARTITION_DATA))
        LONG (ADDR(.ER_TFM_SP_SECURE_TEST_PARTITION_DATA))
        LONG (SIZEOF(.ER_TFM_SP_SECURE_TEST_PARTITION_DATA)/4)
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */
#ifdef TFM_PARTITION_TEST_CORE_IPC
        LONG (LOADADDR(.ER_TFM_SP_IPC_SERVICE_TEST_DATA))
        LONG (ADDR(.ER_TFM_SP_IPC_SERVICE_TEST_DATA))
        LONG (SIZEOF(.ER_TFM_SP_IPC_SERVICE_TEST_DATA)/4)
#endif /* TFM_PARTITION_TEST_CORE_IPC */
#ifdef TFM_PARTITION_TEST_CORE_IPC
        LONG (LOADADDR(.ER_TFM_SP_IPC_CLIENT_TEST_DATA))
        LONG (ADDR(.ER_TFM_SP_IPC_CLIENT_TEST_DATA))
        LONG (SIZEOF(.ER_TFM_SP_IPC_CLIENT_TEST_DATA)/4)
#endif /* TFM_PARTITION_TEST_CORE_IPC */
#ifdef TFM_ENABLE_IRQ_TEST
        LONG (LOADADDR(.ER_TFM_IRQ_TEST_1_DATA))
        LONG (ADDR(.ER_TFM_IRQ_TEST_1_DATA))
        LONG (SIZEOF(.ER_TFM_IRQ_TEST_1_DATA)/4)
#endif /* TFM_ENABLE_IRQ_TEST */
#ifdef TFM_PARTITION_TEST_PS
        LONG (LOADADDR(.ER_TFM_SP_PS_TEST_DATA))
        LONG (ADDR(.ER_TFM_SP_PS_TEST_DATA))
        LONG (SIZEOF(.ER_TFM_SP_PS_TEST_DATA)/4)
#endif /* TFM_PARTITION_TEST_PS */
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
        LONG (LOADADDR(.ER_TFM_SP_SECURE_CLIENT_2_DATA))
        LONG (ADDR(.ER_TFM_SP_SECURE_CLIENT_2_DATA))
        LONG (SIZEOF(.ER_TFM_SP_SECURE_CLIENT_2_DATA)/4)
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */
#ifdef TFM_PARTITION_FIRMWARE_UPDATE
        LONG (LOADADDR(.ER_TFM_SP_FWU_DATA))
        LONG (ADDR(.ER_TFM_SP_FWU_DATA))
        LONG (SIZEOF(.ER_TFM_SP_FWU_DATA)/4)
#endif /* TFM_PARTITION_FIRMWARE_UPDATE */
#ifdef TFM_PARTITION_APP_ROT
        LONG (LOADADDR(.ER_TFM_APP_ROT_DATA))
        LONG (ADDR(.ER_TFM_APP_ROT_DATA))
        LONG (SIZEOF(.ER_TFM_APP_ROT_DATA)/4)
#endif /* TFM_PARTITION_APP_ROT */
#ifdef TFM_PARTITION_FFM11
        LONG (LOADADDR(.ER_TFM_SP_FFM11_DATA))
        LONG (ADDR(.ER_TFM_SP_FFM11_DATA))
        LONG (SIZEOF(.ER_TFM_SP_FFM11_DATA)/4)
#endif /* TFM_PARTITION_FFM11 */

        __copy_table_end__ = .;
    } > FLASH

    .zero.table : ALIGN(4)
    {
        __zero_table_start__ = .;
        LONG (ADDR(.TFM_BSS))
        LONG (SIZEOF(.TFM_BSS)/4)
        LONG (ADDR(.TFM_UNPRIV_BSS))
        LONG (SIZEOF(.TFM_UNPRIV_BSS)/4)
#ifdef TFM_PARTITION_PROTECTED_STORAGE
        LONG (ADDR(.TFM_SP_PS_BSS))
        LONG (SIZEOF(.TFM_SP_PS_BSS)/4)
#endif /* TFM_PARTITION_PROTECTED_STORAGE */
#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
        LONG (ADDR(.TFM_SP_ITS_BSS))
        LONG (SIZEOF(.TFM_SP_ITS_BSS)/4)
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */
#ifdef TFM_PARTITION_CRYPTO
        LONG (ADDR(.TFM_SP_CRYPTO_BSS))
        LONG (SIZEOF(.TFM_SP_CRYPTO_BSS)/4)
#endif /* TFM_PARTITION_CRYPTO */
#ifdef TFM_PARTITION_PLATFORM
        LONG (ADDR(.TFM_SP_PLATFORM_BSS))
        LONG (SIZEOF(.TFM_SP_PLATFORM_BSS)/4)
#endif /* TFM_PARTITION_PLATFORM */
#ifdef TFM_PARTITION_INITIAL_ATTESTATION
        LONG (ADDR(.TFM_SP_INITIAL_ATTESTATION_BSS))
        LONG (SIZEOF(.TFM_SP_INITIAL_ATTESTATION_BSS)/4)
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */
#ifdef TFM_PARTITION_TEST_CORE
        LONG (ADDR(.TFM_SP_CORE_TEST_BSS))
        LONG (SIZEOF(.TFM_SP_CORE_TEST_BSS)/4)
#endif /* TFM_PARTITION_TEST_CORE */
#ifdef TFM_PARTITION_TEST_CORE
        LONG (ADDR(.TFM_SP_CORE_TEST_2_BSS))
        LONG (SIZEOF(.TFM_SP_CORE_TEST_2_BSS)/4)
#endif /* TFM_PARTITION_TEST_CORE */
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
        LONG (ADDR(.TFM_SP_SECURE_TEST_PARTITION_BSS))
        LONG (SIZEOF(.TFM_SP_SECURE_TEST_PARTITION_BSS)/4)
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */
#ifdef TFM_PARTITION_TEST_CORE_IPC
        LONG (ADDR(.TFM_SP_IPC_SERVICE_TEST_BSS))
        LONG (SIZEOF(.TFM_SP_IPC_SERVICE_TEST_BSS)/4)
#endif /* TFM_PARTITION_TEST_CORE_IPC */
#ifdef TFM_PARTITION_TEST_CORE_IPC
        LONG (ADDR(.TFM_SP_IPC_CLIENT_TEST_BSS))
        LONG (SIZEOF(.TFM_SP_IPC_CLIENT_TEST_BSS)/4)
#endif /* TFM_PARTITION_TEST_CORE_IPC */
#ifdef TFM_ENABLE_IRQ_TEST
        LONG (ADDR(.TFM_IRQ_TEST_1_BSS))
        LONG (SIZEOF(.TFM_IRQ_TEST_1_BSS)/4)
#endif /* TFM_ENABLE_IRQ_TEST */
#ifdef TFM_PARTITION_TEST_PS
        LONG (ADDR(.TFM_SP_PS_TEST_BSS))
        LONG (SIZEOF(.TFM_SP_PS_TEST_BSS)/4)
#endif /* TFM_PARTITION_TEST_PS */
#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
        LONG (ADDR(.TFM_SP_SECURE_CLIENT_2_BSS))
        LONG (SIZEOF(.TFM_SP_SECURE_CLIENT_2_BSS)/4)
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */
#ifdef TFM_PARTITION_FIRMWARE_UPDATE
        LONG (ADDR(.TFM_SP_FWU_BSS))
        LONG (SIZEOF(.TFM_SP_FWU_BSS)/4)
#endif /* TFM_PARTITION_FIRMWARE_UPDATE */
#ifdef TFM_PARTITION_APP_ROT
        LONG (ADDR(.TFM_APP_ROT_BSS))
        LONG (SIZEOF(.TFM_APP_ROT_BSS)/4)
#endif /* TFM_PARTITION_APP_ROT */
#ifdef TFM_PARTITION_FFM11
        LONG (ADDR(.TFM_SP_FFM11_BSS))
        LONG (SIZEOF(.TFM_SP_FFM11_BSS)/4)
#endif /* TFM_PARTITION_FFM11 */
        __zero_table_end__ = .;
    } > FLASH

    /**** PSA RoT RO CODE + RO-data starts here */
#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
    .TFM_SP_ITS_RO : ALIGN(32)
    {
        *tfm_internal_trusted_storage.*(.text*)
        *tfm_internal_trusted_storage.*:*(.rodata*)
        *its_flash_*.*(.text*)
        *its_flash_*.*:*(.rodata*)
        *its_utils.*(.text*)
        *its_utils.*:*(.rodata*)
        *tfm_its_req_mngr.*(.text*)
        *tfm_its_req_mngr.*:*(.rodata*)
        *(TFM_SP_ITS_PSA-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_ITS_RO$$Base = ADDR(.TFM_SP_ITS_RO);
    Image$$TFM_SP_ITS_RO$$Limit = ADDR(.TFM_SP_ITS_RO) + SIZEOF(.TFM_SP_ITS_RO);
#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

#ifdef TFM_PARTITION_CRYPTO
    .TFM_SP_CRYPTO_RO : ALIGN(32)
    {
        */crypto*.o(.text*)
        */crypto*.o:*(.rodata*)
        */platform.o(.text*)
        */platform.o:*(.rodata*)
        */platform_util.o(.text*)
        */platform_util.o:*(.rodata*)
        *(TFM_SP_CRYPTO_PSA-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_CRYPTO_RO$$Base = ADDR(.TFM_SP_CRYPTO_RO);
    Image$$TFM_SP_CRYPTO_RO$$Limit = ADDR(.TFM_SP_CRYPTO_RO) + SIZEOF(.TFM_SP_CRYPTO_RO);
#endif /* TFM_PARTITION_CRYPTO */

#ifdef TFM_PARTITION_PLATFORM
    .TFM_SP_PLATFORM_RO : ALIGN(32)
    {
        *platform_sp.o(.text*)
        *platform_sp.o:*(.rodata*)
        *tfm_plaform_system.o(.text*)
        *tfm_plaform_system.o:*(.rodata*)
        *tfm_platform.o(.text*)
        *tfm_platform.o:*(.rodata*)
        *(TFM_SP_PLATFORM_PSA-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_PLATFORM_RO$$Base = ADDR(.TFM_SP_PLATFORM_RO);
    Image$$TFM_SP_PLATFORM_RO$$Limit = ADDR(.TFM_SP_PLATFORM_RO) + SIZEOF(.TFM_SP_PLATFORM_RO);
#endif /* TFM_PARTITION_PLATFORM */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION
    .TFM_SP_INITIAL_ATTESTATION_RO : ALIGN(32)
    {
        *tfm_partition_attestation.*(.text*)
        *tfm_partition_attestation.*:*(.rodata*)
        *UsefulBuff.*(.text*)
        *UsefulBuff.*:*(.rodata*)
        *(TFM_SP_INITIAL_ATTESTATION_PSA-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_INITIAL_ATTESTATION_RO$$Base = ADDR(.TFM_SP_INITIAL_ATTESTATION_RO);
    Image$$TFM_SP_INITIAL_ATTESTATION_RO$$Limit = ADDR(.TFM_SP_INITIAL_ATTESTATION_RO) + SIZEOF(.TFM_SP_INITIAL_ATTESTATION_RO);
#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_TEST_CORE
    .TFM_SP_CORE_TEST_RO : ALIGN(32)
    {
        *tfm_ss_core_test.*(.text*)
        *tfm_ss_core_test.*:*(.rodata*)
        *(TFM_SP_CORE_TEST_PSA-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_CORE_TEST_RO$$Base = ADDR(.TFM_SP_CORE_TEST_RO);
    Image$$TFM_SP_CORE_TEST_RO$$Limit = ADDR(.TFM_SP_CORE_TEST_RO) + SIZEOF(.TFM_SP_CORE_TEST_RO);
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    .TFM_SP_SECURE_TEST_PARTITION_RO : ALIGN(32)
    {
        *tfm_secure_client_service.*(.text*)
        *tfm_secure_client_service.*:*(.rodata*)
        *test_framework*(.text*)
        *test_framework*:*(.rodata*)
        *attestation_s_interface_testsuite.*(.text*)
        *attestation_s_interface_testsuite.*:*(.rodata*)
        *secure_suites.*(.text*)
        *secure_suites.*:*(.rodata*)
        *Driver_USART.*(.text*)
        *Driver_USART.*:*(.rodata*)
        *(TFM_SP_SECURE_TEST_PARTITION_PSA-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_SECURE_TEST_PARTITION_RO$$Base = ADDR(.TFM_SP_SECURE_TEST_PARTITION_RO);
    Image$$TFM_SP_SECURE_TEST_PARTITION_RO$$Limit = ADDR(.TFM_SP_SECURE_TEST_PARTITION_RO) + SIZEOF(.TFM_SP_SECURE_TEST_PARTITION_RO);
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_TEST_CORE_IPC
    .TFM_SP_IPC_SERVICE_TEST_RO : ALIGN(32)
    {
        *ipc_service_test.*(.text*)
        *ipc_service_test.*:*(.rodata*)
        *(TFM_SP_IPC_SERVICE_TEST_PSA-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_IPC_SERVICE_TEST_RO$$Base = ADDR(.TFM_SP_IPC_SERVICE_TEST_RO);
    Image$$TFM_SP_IPC_SERVICE_TEST_RO$$Limit = ADDR(.TFM_SP_IPC_SERVICE_TEST_RO) + SIZEOF(.TFM_SP_IPC_SERVICE_TEST_RO);
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_PARTITION_TEST_PS
    .TFM_SP_PS_TEST_RO : ALIGN(32)
    {
        *tfm_*partition_ps_test.*(.text*)
        *tfm_*partition_ps_test.*:*(.rodata*)
        *(TFM_SP_PS_TEST_PSA-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_PS_TEST_RO$$Base = ADDR(.TFM_SP_PS_TEST_RO);
    Image$$TFM_SP_PS_TEST_RO$$Limit = ADDR(.TFM_SP_PS_TEST_RO) + SIZEOF(.TFM_SP_PS_TEST_RO);
#endif /* TFM_PARTITION_TEST_PS */

#ifdef TFM_PARTITION_FIRMWARE_UPDATE
    .TFM_SP_FWU_RO : ALIGN(32)
    {
        *tfm_mcuboot_fwu.*(.text*)
        *tfm_mcuboot_fwu.*:*(.rodata*)
        *tfm_firmware_update.*(.text*)
        *tfm_firmware_update.*:*(.rodata*)
        *tfm_fwu*.*(.text*)
        *tfm_fwu*.*:*(.rodata*)
        *(TFM_SP_FWU_PSA-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_FWU_RO$$Base = ADDR(.TFM_SP_FWU_RO);
    Image$$TFM_SP_FWU_RO$$Limit = ADDR(.TFM_SP_FWU_RO) + SIZEOF(.TFM_SP_FWU_RO);
#endif /* TFM_PARTITION_FIRMWARE_UPDATE */

    /**** PSA RoT RO CODE + data ends here */
/**** Unprivileged Secure code starts here */
    .ER_UNPRIV_CODE : ALIGN(GTZC_FLASH_ALIGN)
    {
    */tfm_spm_services.o(.text*)
    */tfm_*_secure_api.o(.text*)
    */service_api.o(.text*)
    */sprt*.o(.text*)
    *(SFN)
    *psa_client.o(.text*)
    */psa_service.o(.text*)
    */psa_lifecycle.o(.text*)
    */tfm_log_raw.o(.text*)
    */stm32u5xx_hal*.o(.text*)
    *libc_nano*:*(.text*)
    *libc_nano*:*(.rodata*)
    *libgcc*:*(.text*)
    *libgcc*:*(.rodata*)
    */stm32u5xx_hal.*(.text*)
    */stm32u5xx_hal.*(.rodata*)
    */tick.*(.text*)
    */tick.*(.rodata*)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_UNPRIV_CODE$$RO$$Base = ADDR(.ER_UNPRIV_CODE);
    Image$$TFM_UNPRIV_CODE$$RO$$Limit = ADDR(.ER_UNPRIV_CODE) + SIZEOF(.ER_UNPRIV_CODE);

    /**** APPLICATION RoT RO CODE + data starts here */
#ifdef TFM_PARTITION_PROTECTED_STORAGE
    .TFM_SP_PS_RO : ALIGN(32)
    {
        *ps*.o(.text*)
        *ps*.o:*(.rodata*)
        *protected_storage*.o(.text*)
        *protected_storage*.o:*(.rodata*)
        *tfm_ps_req*.o(.text*)
        *tfm_ps_req*.o:*(.rodata*)
        *(TFM_SP_PS_APP-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_PS_CODE$$Base = ADDR(.TFM_SP_PS_RO);
    Image$$TFM_SP_PS_CODE$$Limit = ADDR(.TFM_SP_PS_RO) + SIZEOF(.TFM_SP_PS_RO);
#endif /* TFM_PARTITION_PROTECTED_STORAGE */

#ifdef TFM_PARTITION_TEST_CORE
    .TFM_SP_CORE_TEST_2_RO : ALIGN(32)
    {
        *tfm_ss_core_test_2(.text*)
        *tfm_ss_core_test_2:*(.rodata*)
        *(TFM_SP_CORE_TEST_2_APP-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_CORE_TEST_2_CODE$$Base = ADDR(.TFM_SP_CORE_TEST_2_RO);
    Image$$TFM_SP_CORE_TEST_2_CODE$$Limit = ADDR(.TFM_SP_CORE_TEST_2_RO) + SIZEOF(.TFM_SP_CORE_TEST_2_RO);
#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE_IPC
    .TFM_SP_IPC_CLIENT_TEST_RO : ALIGN(32)
    {
        *ipc_client_test.*(.text*)
        *ipc_client_test.*:*(.rodata*)
        *(TFM_SP_IPC_CLIENT_TEST_APP-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_IPC_CLIENT_TEST_CODE$$Base = ADDR(.TFM_SP_IPC_CLIENT_TEST_RO);
    Image$$TFM_SP_IPC_CLIENT_TEST_CODE$$Limit = ADDR(.TFM_SP_IPC_CLIENT_TEST_RO) + SIZEOF(.TFM_SP_IPC_CLIENT_TEST_RO);
#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_ENABLE_IRQ_TEST
    .TFM_IRQ_TEST_1_RO : ALIGN(32)
    {
        *tfm_irq_test_service_1.*(.text*)
        *tfm_irq_test_service_1.*:*(.rodata*)
        *timer_cmsdk*(.text*)
        *timer_cmsdk*:*(.rodata*)
        *(TFM_IRQ_TEST_1_APP-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_IRQ_TEST_1_CODE$$Base = ADDR(.TFM_IRQ_TEST_1_RO);
    Image$$TFM_IRQ_TEST_1_CODE$$Limit = ADDR(.TFM_IRQ_TEST_1_RO) + SIZEOF(.TFM_IRQ_TEST_1_RO);
#endif /* TFM_ENABLE_IRQ_TEST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    .TFM_SP_SECURE_CLIENT_2_RO : ALIGN(32)
    {
        *tfm_*partition_secure_client_2.*(.text*)
        *tfm_*partition_secure_client_2.*:*(.rodata*)
        *(TFM_SP_SECURE_CLIENT_2_APP-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_SECURE_CLIENT_2_CODE$$Base = ADDR(.TFM_SP_SECURE_CLIENT_2_RO);
    Image$$TFM_SP_SECURE_CLIENT_2_CODE$$Limit = ADDR(.TFM_SP_SECURE_CLIENT_2_RO) + SIZEOF(.TFM_SP_SECURE_CLIENT_2_RO);
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_APP_ROT
    .TFM_APP_ROT_RO : ALIGN(32)
    {
        *app_rot*.o(.text*)
        *app_rot*.o:*(.rodata*)
        *(TFM_APP_ROT_APP-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_APP_ROT_CODE$$Base = ADDR(.TFM_APP_ROT_RO);
    Image$$TFM_APP_ROT_CODE$$Limit = ADDR(.TFM_APP_ROT_RO) + SIZEOF(.TFM_APP_ROT_RO);
#endif /* TFM_PARTITION_APP_ROT */

#ifdef TFM_PARTITION_FFM11
    .TFM_SP_FFM11_RO : ALIGN(32)
    {
        *tfm_*partition_ffm11.*(.text*)
        *tfm_*partition_ffm11.*:*(.rodata*)
        *(TFM_SP_FFM11_APP-ROT_ATTR_FN)
        . = ALIGN(32);
    } > FLASH
    Image$$TFM_SP_FFM11_CODE$$Base = ADDR(.TFM_SP_FFM11_RO);
    Image$$TFM_SP_FFM11_CODE$$Limit = ADDR(.TFM_SP_FFM11_RO) + SIZEOF(.TFM_SP_FFM11_RO);
#endif /* TFM_PARTITION_FFM11 */

    /**** APPLICATION RoT RO CODE ends here */

      .ER_TFM_CODE : ALIGN(4)
    {
        *(.text*)

        KEEP(*(.init))
        KEEP(*(.fini))


        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
         *crtbegin.o(.dtors)
         *crtbegin?.o(.dtors)
         *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
         *(SORT(.dtors.*))
         *(.dtors)

        *(.rodata*)

        KEEP(*(.eh_frame*))
    } > FLASH
    /* Position tag */
    . = ALIGN(32);
    Image$$PT_RO_START$$Base = .;


    /* Position tag */
    . = ALIGN(32);
    Image$$PT_RO_END$$Base = .;

    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH
    __exidx_end = .;

    /**** Base address of secure data area */
    .tfm_secure_data_start :
    {
        . = ABSOLUTE(S_DATA_START+BOOT_SHARED_DATA_SIZE) ;
    } > RAM

   /* Position tag */
    . = ALIGN(32);
    Image$$PT_PRIV_DATA_START$$Base = .;

    .msp_stack : ALIGN(32)
    {
        . += __msp_init_stack_size__;
    } > RAM
    Image$$ARM_LIB_STACK_MSP$$ZI$$Base = ADDR(.msp_stack);
    Image$$ARM_LIB_STACK_MSP$$ZI$$Limit = ADDR(.msp_stack) + SIZEOF(.msp_stack);

    .TFM_DATA : ALIGN(4)
    {
        *(EXCLUDE_FILE(*app_rot*.o *protected_storage.o *ps_*.o) .data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);

    } > RAM AT> FLASH
    Image$$ER_TFM_DATA$$RW$$Base = ADDR(.TFM_DATA);
    Image$$ER_TFM_DATA$$RW$$Limit = ADDR(.TFM_DATA) + SIZEOF(.TFM_DATA);

    .TFM_BSS : ALIGN(4)
    {
        __bss_start__ = .;
        *(EXCLUDE_FILE(*app_rot*.o *protected_storage.o *ps_*.o) .bss*)
        *(EXCLUDE_FILE(*app_rot*.o *protected_storage.o *ps_*.o) COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM AT> RAM
    Image$$ER_TFM_DATA$$ZI$$Base = ADDR(.TFM_BSS);
    Image$$ER_TFM_DATA$$ZI$$Limit = ADDR(.TFM_BSS) + SIZEOF(.TFM_BSS);

    Image$$ER_TFM_DATA$$Base = ADDR(.TFM_DATA);
    Image$$ER_TFM_DATA$$Limit = ADDR(.TFM_DATA) + SIZEOF(.TFM_DATA) + SIZEOF(.TFM_BSS);
    /**** PSA RoT DATA start here */
#ifdef TFM_PARTITION_INTERNAL_TRUSTED_STORAGE

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_ITS_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_ITS_DATA : ALIGN(32)
    {
        *tfm_internal_trusted_storage.*(.data*)
        *its_flash_*.*(.data*)
        *its_utils.*(.data*)
        *tfm_its_req_mngr.*(.data*)
        *(TFM_SP_ITS_PSA-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$ER_TFM_SP_ITS_DATA$$RW$$Base = ADDR(.ER_TFM_SP_ITS_DATA);
    Image$$ER_TFM_SP_ITS_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_ITS_DATA) + SIZEOF(.ER_TFM_SP_ITS_DATA);

    .TFM_SP_ITS_BSS : ALIGN(32)
    {
        start_of_TFM_SP_ITS_DATA = .;
        *tfm_internal_trusted_storage.*(.bss*)
        *tfm_internal_trusted_storage.*(COMMON)
        *its_flash_*.*(.bss*)
        *its_flash_*.*(COMMON)
        *its_utils.*(.bss*)
        *its_utils.*(COMMON)
        *tfm_its_req_mngr.*(.bss*)
        *tfm_its_req_mngr.*(COMMON)
        *(TFM_SP_ITS_PSA-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_ITS_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_ITS_DATA$$ZI$$Base = ADDR(.TFM_SP_ITS_BSS);
    Image$$ER_TFM_SP_ITS_DATA$$ZI$$Limit = ADDR(.TFM_SP_ITS_BSS) + SIZEOF(.TFM_SP_ITS_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_ITS_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_INTERNAL_TRUSTED_STORAGE */

#ifdef TFM_PARTITION_CRYPTO

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_CRYPTO_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_CRYPTO_DATA : ALIGN(32)
    {
        */crypto*.o(.data*)
        */platform.o(.data*)
        */platform_util.o(.data*)
        *(TFM_SP_CRYPTO_PSA-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$ER_TFM_SP_CRYPTO_DATA$$RW$$Base = ADDR(.ER_TFM_SP_CRYPTO_DATA);
    Image$$ER_TFM_SP_CRYPTO_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_CRYPTO_DATA) + SIZEOF(.ER_TFM_SP_CRYPTO_DATA);

    .TFM_SP_CRYPTO_BSS : ALIGN(32)
    {
        start_of_TFM_SP_CRYPTO_DATA = .;
        */crypto*.o(.bss*)
        */crypto*.o(COMMON)
        */platform.o(.bss*)
        */platform.o(COMMON)
        */platform_util.o(.bss*)
        */platform_util.o(COMMON)
        *(TFM_SP_CRYPTO_PSA-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_CRYPTO_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_CRYPTO_DATA$$ZI$$Base = ADDR(.TFM_SP_CRYPTO_BSS);
    Image$$ER_TFM_SP_CRYPTO_DATA$$ZI$$Limit = ADDR(.TFM_SP_CRYPTO_BSS) + SIZEOF(.TFM_SP_CRYPTO_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_CRYPTO_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_CRYPTO */

#ifdef TFM_PARTITION_PLATFORM

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_PLATFORM_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_PLATFORM_DATA : ALIGN(32)
    {
        *platform_sp.o(.data*)
        *tfm_plaform_system.o(.data*)
        *tfm_platform.o(.data*)
        *(TFM_SP_PLATFORM_PSA-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$ER_TFM_SP_PLATFORM_DATA$$RW$$Base = ADDR(.ER_TFM_SP_PLATFORM_DATA);
    Image$$ER_TFM_SP_PLATFORM_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_PLATFORM_DATA) + SIZEOF(.ER_TFM_SP_PLATFORM_DATA);

    .TFM_SP_PLATFORM_BSS : ALIGN(32)
    {
        start_of_TFM_SP_PLATFORM_DATA = .;
        *platform_sp.o(.bss*)
        *platform_sp.o(COMMON)
        *tfm_plaform_system.o(.bss*)
        *tfm_plaform_system.o(COMMON)
        *tfm_platform.o(.bss*)
        *tfm_platform.o(COMMON)
        *(TFM_SP_PLATFORM_PSA-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_PLATFORM_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_PLATFORM_DATA$$ZI$$Base = ADDR(.TFM_SP_PLATFORM_BSS);
    Image$$ER_TFM_SP_PLATFORM_DATA$$ZI$$Limit = ADDR(.TFM_SP_PLATFORM_BSS) + SIZEOF(.TFM_SP_PLATFORM_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_PLATFORM_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_PLATFORM */

#ifdef TFM_PARTITION_INITIAL_ATTESTATION

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_INITIAL_ATTESTATION_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_INITIAL_ATTESTATION_DATA : ALIGN(32)
    {
        *tfm_partition_attestation.*(.data*)
        *UsefulBuff.*(.data*)
        *(TFM_SP_INITIAL_ATTESTATION_PSA-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$ER_TFM_SP_INITIAL_ATTESTATION_DATA$$RW$$Base = ADDR(.ER_TFM_SP_INITIAL_ATTESTATION_DATA);
    Image$$ER_TFM_SP_INITIAL_ATTESTATION_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_INITIAL_ATTESTATION_DATA) + SIZEOF(.ER_TFM_SP_INITIAL_ATTESTATION_DATA);

    .TFM_SP_INITIAL_ATTESTATION_BSS : ALIGN(32)
    {
        start_of_TFM_SP_INITIAL_ATTESTATION_DATA = .;
        *tfm_partition_attestation.*(.bss*)
        *tfm_partition_attestation.*(COMMON)
        *UsefulBuff.*(.bss*)
        *UsefulBuff.*(COMMON)
        *(TFM_SP_INITIAL_ATTESTATION_PSA-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_INITIAL_ATTESTATION_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_INITIAL_ATTESTATION_DATA$$ZI$$Base = ADDR(.TFM_SP_INITIAL_ATTESTATION_BSS);
    Image$$ER_TFM_SP_INITIAL_ATTESTATION_DATA$$ZI$$Limit = ADDR(.TFM_SP_INITIAL_ATTESTATION_BSS) + SIZEOF(.TFM_SP_INITIAL_ATTESTATION_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_INITIAL_ATTESTATION_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_INITIAL_ATTESTATION */

#ifdef TFM_PARTITION_TEST_CORE

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_CORE_TEST_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_CORE_TEST_DATA : ALIGN(32)
    {
        *tfm_ss_core_test.*(.data*)
        *(TFM_SP_CORE_TEST_PSA-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$ER_TFM_SP_CORE_TEST_DATA$$RW$$Base = ADDR(.ER_TFM_SP_CORE_TEST_DATA);
    Image$$ER_TFM_SP_CORE_TEST_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_CORE_TEST_DATA) + SIZEOF(.ER_TFM_SP_CORE_TEST_DATA);

    .TFM_SP_CORE_TEST_BSS : ALIGN(32)
    {
        start_of_TFM_SP_CORE_TEST_DATA = .;
        *tfm_ss_core_test.*(.bss*)
        *tfm_ss_core_test.*(COMMON)
        *(TFM_SP_CORE_TEST_PSA-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_CORE_TEST_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_CORE_TEST_DATA$$ZI$$Base = ADDR(.TFM_SP_CORE_TEST_BSS);
    Image$$ER_TFM_SP_CORE_TEST_DATA$$ZI$$Limit = ADDR(.TFM_SP_CORE_TEST_BSS) + SIZEOF(.TFM_SP_CORE_TEST_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_CORE_TEST_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_SECURE_TEST_PARTITION_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_SECURE_TEST_PARTITION_DATA : ALIGN(32)
    {
        *tfm_secure_client_service.*(.data*)
        *test_framework*(.data*)
        *attestation_s_interface_testsuite.*(.data*)
        *secure_suites.*(.data*)
        *Driver_USART.*(.data*)
        *(TFM_SP_SECURE_TEST_PARTITION_PSA-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$ER_TFM_SP_SECURE_TEST_PARTITION_DATA$$RW$$Base = ADDR(.ER_TFM_SP_SECURE_TEST_PARTITION_DATA);
    Image$$ER_TFM_SP_SECURE_TEST_PARTITION_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_SECURE_TEST_PARTITION_DATA) + SIZEOF(.ER_TFM_SP_SECURE_TEST_PARTITION_DATA);

    .TFM_SP_SECURE_TEST_PARTITION_BSS : ALIGN(32)
    {
        start_of_TFM_SP_SECURE_TEST_PARTITION_DATA = .;
        *tfm_secure_client_service.*(.bss*)
        *tfm_secure_client_service.*(COMMON)
        *test_framework*(.bss*)
        *test_framework*(COMMON)
        *attestation_s_interface_testsuite.*(.bss*)
        *attestation_s_interface_testsuite.*(COMMON)
        *secure_suites.*(.bss*)
        *secure_suites.*(COMMON)
        *Driver_USART.*(.bss*)
        *Driver_USART.*(COMMON)
        *(TFM_SP_SECURE_TEST_PARTITION_PSA-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_SECURE_TEST_PARTITION_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_SECURE_TEST_PARTITION_DATA$$ZI$$Base = ADDR(.TFM_SP_SECURE_TEST_PARTITION_BSS);
    Image$$ER_TFM_SP_SECURE_TEST_PARTITION_DATA$$ZI$$Limit = ADDR(.TFM_SP_SECURE_TEST_PARTITION_BSS) + SIZEOF(.TFM_SP_SECURE_TEST_PARTITION_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_SECURE_TEST_PARTITION_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_TEST_CORE_IPC

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_IPC_SERVICE_TEST_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_IPC_SERVICE_TEST_DATA : ALIGN(32)
    {
        *ipc_service_test.*(.data*)
        *(TFM_SP_IPC_SERVICE_TEST_PSA-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$ER_TFM_SP_IPC_SERVICE_TEST_DATA$$RW$$Base = ADDR(.ER_TFM_SP_IPC_SERVICE_TEST_DATA);
    Image$$ER_TFM_SP_IPC_SERVICE_TEST_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_IPC_SERVICE_TEST_DATA) + SIZEOF(.ER_TFM_SP_IPC_SERVICE_TEST_DATA);

    .TFM_SP_IPC_SERVICE_TEST_BSS : ALIGN(32)
    {
        start_of_TFM_SP_IPC_SERVICE_TEST_DATA = .;
        *ipc_service_test.*(.bss*)
        *ipc_service_test.*(COMMON)
        *(TFM_SP_IPC_SERVICE_TEST_PSA-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_IPC_SERVICE_TEST_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_IPC_SERVICE_TEST_DATA$$ZI$$Base = ADDR(.TFM_SP_IPC_SERVICE_TEST_BSS);
    Image$$ER_TFM_SP_IPC_SERVICE_TEST_DATA$$ZI$$Limit = ADDR(.TFM_SP_IPC_SERVICE_TEST_BSS) + SIZEOF(.TFM_SP_IPC_SERVICE_TEST_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_IPC_SERVICE_TEST_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_PARTITION_TEST_PS

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_PS_TEST_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_PS_TEST_DATA : ALIGN(32)
    {
        *tfm_*partition_ps_test.*(.data*)
        *(TFM_SP_PS_TEST_PSA-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$ER_TFM_SP_PS_TEST_DATA$$RW$$Base = ADDR(.ER_TFM_SP_PS_TEST_DATA);
    Image$$ER_TFM_SP_PS_TEST_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_PS_TEST_DATA) + SIZEOF(.ER_TFM_SP_PS_TEST_DATA);

    .TFM_SP_PS_TEST_BSS : ALIGN(32)
    {
        start_of_TFM_SP_PS_TEST_DATA = .;
        *tfm_*partition_ps_test.*(.bss*)
        *tfm_*partition_ps_test.*(COMMON)
        *(TFM_SP_PS_TEST_PSA-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_PS_TEST_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_PS_TEST_DATA$$ZI$$Base = ADDR(.TFM_SP_PS_TEST_BSS);
    Image$$ER_TFM_SP_PS_TEST_DATA$$ZI$$Limit = ADDR(.TFM_SP_PS_TEST_BSS) + SIZEOF(.TFM_SP_PS_TEST_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_PS_TEST_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_TEST_PS */

#ifdef TFM_PARTITION_FIRMWARE_UPDATE

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_FWU_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_FWU_DATA : ALIGN(32)
    {
        *tfm_mcuboot_fwu.*(.data*)
        *tfm_firmware_update.*(.data*)
        *tfm_fwu*.*(.data*)
        *(TFM_SP_FWU_PSA-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$ER_TFM_SP_FWU_DATA$$RW$$Base = ADDR(.ER_TFM_SP_FWU_DATA);
    Image$$ER_TFM_SP_FWU_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_FWU_DATA) + SIZEOF(.ER_TFM_SP_FWU_DATA);

    .TFM_SP_FWU_BSS : ALIGN(32)
    {
        start_of_TFM_SP_FWU_DATA = .;
        *tfm_mcuboot_fwu.*(.bss*)
        *tfm_mcuboot_fwu.*(COMMON)
        *tfm_firmware_update.*(.bss*)
        *tfm_firmware_update.*(COMMON)
        *tfm_fwu*.*(.bss*)
        *tfm_fwu*.*(COMMON)
        *(TFM_SP_FWU_PSA-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_FWU_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_FWU_DATA$$ZI$$Base = ADDR(.TFM_SP_FWU_BSS);
    Image$$ER_TFM_SP_FWU_DATA$$ZI$$Limit = ADDR(.TFM_SP_FWU_BSS) + SIZEOF(.TFM_SP_FWU_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_FWU_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_FIRMWARE_UPDATE */


    /**** PSA RoT DATA end here */
    .TFM_PSA_RW_STACK_END : ALIGN(GTZC_RAM_ALIGN)
    {
        . += 0x0;
    } > RAM
    /* Position tag */
    Image$$TFM_PSA_RW_STACK_END$$Base = ADDR(.TFM_PSA_RW_STACK_END);

    /* PSP is unprivileged in single-core topology */
    .psp_stack : ALIGN(GTZC_RAM_ALIGN)
    {
        . += (__psp_stack_size__);
    } > RAM
    Image$$ARM_LIB_STACK$$ZI$$Base = ADDR(.psp_stack);
    Image$$ARM_LIB_STACK$$ZI$$Limit = ADDR(.psp_stack) + SIZEOF(.psp_stack);

    .TFM_UNPRIV_DATA : ALIGN(32)
    {
        stm32u5xx_hal*.o(.data*)
        *tick.o(.data*)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$TFM_UNPRIV_DATA$$RW$$Base = ADDR(.TFM_UNPRIV_DATA);
    Image$$TFM_UNPRIV_DATA$$RW$$Limit = ADDR(.TFM_UNPRIV_DATA) + SIZEOF(.TFM_UNPRIV_DATA);

    .TFM_UNPRIV_BSS : ALIGN(32)
    {
        stm32u5xx_hal*.o(.bss*)
        *tick.o(.bss*)
        stm32u5xx_hal*.o(COMMON)
        *tick.o(COMMON)
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$TFM_UNPRIV_DATA$$ZI$$Base = ADDR(.TFM_UNPRIV_BSS);
    Image$$TFM_UNPRIV_DATA$$ZI$$Limit = ADDR(.TFM_UNPRIV_BSS) + SIZEOF(.TFM_UNPRIV_BSS);

    /**** APPLICATION RoT DATA start here */
#ifdef TFM_PARTITION_PROTECTED_STORAGE

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_PS_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_PS_DATA : ALIGN(32)
    {
        *ps*.o(.data*)
        *protected_storage*.o(.data*)
        *tfm_ps_req*.o(.data*)
        *(TFM_SP_PS_APP-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$.ER_TFM_SP_PS_DATA$$RW$$Base = ADDR(.ER_TFM_SP_PS_DATA);
    Image$$.ER_TFM_SP_PS_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_PS_DATA) + SIZEOF(.ER_TFM_SP_PS_DATA);

    .TFM_SP_PS_BSS : ALIGN(32)
    {
        start_of_TFM_SP_PS_DATA = .;
        *ps*.o(.bss*)
        *ps*.o(COMMON)
        *protected_storage*.o(.bss*)
        *protected_storage*.o(COMMON)
        *tfm_ps_req*.o(.bss*)
        *tfm_ps_req*.o(COMMON)
        *(TFM_SP_PS_APP-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_PS_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_PS_DATA$$ZI$$Base = ADDR(.TFM_SP_PS_BSS);
    Image$$ER_TFM_SP_PS_DATA$$ZI$$Limit = ADDR(.TFM_SP_PS_BSS) + SIZEOF(.TFM_SP_PS_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_PS_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_PROTECTED_STORAGE */

#ifdef TFM_PARTITION_TEST_CORE

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_CORE_TEST_2_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_CORE_TEST_2_DATA : ALIGN(32)
    {
        *tfm_ss_core_test_2(.data*)
        *(TFM_SP_CORE_TEST_2_APP-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$.ER_TFM_SP_CORE_TEST_2_DATA$$RW$$Base = ADDR(.ER_TFM_SP_CORE_TEST_2_DATA);
    Image$$.ER_TFM_SP_CORE_TEST_2_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_CORE_TEST_2_DATA) + SIZEOF(.ER_TFM_SP_CORE_TEST_2_DATA);

    .TFM_SP_CORE_TEST_2_BSS : ALIGN(32)
    {
        start_of_TFM_SP_CORE_TEST_2_DATA = .;
        *tfm_ss_core_test_2(.bss*)
        *tfm_ss_core_test_2(COMMON)
        *(TFM_SP_CORE_TEST_2_APP-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_CORE_TEST_2_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_CORE_TEST_2_DATA$$ZI$$Base = ADDR(.TFM_SP_CORE_TEST_2_BSS);
    Image$$ER_TFM_SP_CORE_TEST_2_DATA$$ZI$$Limit = ADDR(.TFM_SP_CORE_TEST_2_BSS) + SIZEOF(.TFM_SP_CORE_TEST_2_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_CORE_TEST_2_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_TEST_CORE */

#ifdef TFM_PARTITION_TEST_CORE_IPC

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_IPC_CLIENT_TEST_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_IPC_CLIENT_TEST_DATA : ALIGN(32)
    {
        *ipc_client_test.*(.data*)
        *(TFM_SP_IPC_CLIENT_TEST_APP-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$.ER_TFM_SP_IPC_CLIENT_TEST_DATA$$RW$$Base = ADDR(.ER_TFM_SP_IPC_CLIENT_TEST_DATA);
    Image$$.ER_TFM_SP_IPC_CLIENT_TEST_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_IPC_CLIENT_TEST_DATA) + SIZEOF(.ER_TFM_SP_IPC_CLIENT_TEST_DATA);

    .TFM_SP_IPC_CLIENT_TEST_BSS : ALIGN(32)
    {
        start_of_TFM_SP_IPC_CLIENT_TEST_DATA = .;
        *ipc_client_test.*(.bss*)
        *ipc_client_test.*(COMMON)
        *(TFM_SP_IPC_CLIENT_TEST_APP-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_IPC_CLIENT_TEST_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_IPC_CLIENT_TEST_DATA$$ZI$$Base = ADDR(.TFM_SP_IPC_CLIENT_TEST_BSS);
    Image$$ER_TFM_SP_IPC_CLIENT_TEST_DATA$$ZI$$Limit = ADDR(.TFM_SP_IPC_CLIENT_TEST_BSS) + SIZEOF(.TFM_SP_IPC_CLIENT_TEST_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_IPC_CLIENT_TEST_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_TEST_CORE_IPC */

#ifdef TFM_ENABLE_IRQ_TEST

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_IRQ_TEST_1_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_IRQ_TEST_1_DATA : ALIGN(32)
    {
        *tfm_irq_test_service_1.*(.data*)
        *timer_cmsdk*(.data*)
        *(TFM_IRQ_TEST_1_APP-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$.ER_TFM_IRQ_TEST_1_DATA$$RW$$Base = ADDR(.ER_TFM_IRQ_TEST_1_DATA);
    Image$$.ER_TFM_IRQ_TEST_1_DATA$$RW$$Limit = ADDR(.ER_TFM_IRQ_TEST_1_DATA) + SIZEOF(.ER_TFM_IRQ_TEST_1_DATA);

    .TFM_IRQ_TEST_1_BSS : ALIGN(32)
    {
        start_of_TFM_IRQ_TEST_1_DATA = .;
        *tfm_irq_test_service_1.*(.bss*)
        *tfm_irq_test_service_1.*(COMMON)
        *timer_cmsdk*(.bss*)
        *timer_cmsdk*(COMMON)
        *(TFM_IRQ_TEST_1_APP-ROT_ATTR_ZI)
        . += (. - start_of_TFM_IRQ_TEST_1_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_IRQ_TEST_1_DATA$$ZI$$Base = ADDR(.TFM_IRQ_TEST_1_BSS);
    Image$$ER_TFM_IRQ_TEST_1_DATA$$ZI$$Limit = ADDR(.TFM_IRQ_TEST_1_BSS) + SIZEOF(.TFM_IRQ_TEST_1_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_IRQ_TEST_1_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_ENABLE_IRQ_TEST */

#ifdef TFM_PARTITION_TEST_SECURE_SERVICES

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_SECURE_CLIENT_2_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_SECURE_CLIENT_2_DATA : ALIGN(32)
    {
        *tfm_*partition_secure_client_2.*(.data*)
        *(TFM_SP_SECURE_CLIENT_2_APP-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$.ER_TFM_SP_SECURE_CLIENT_2_DATA$$RW$$Base = ADDR(.ER_TFM_SP_SECURE_CLIENT_2_DATA);
    Image$$.ER_TFM_SP_SECURE_CLIENT_2_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_SECURE_CLIENT_2_DATA) + SIZEOF(.ER_TFM_SP_SECURE_CLIENT_2_DATA);

    .TFM_SP_SECURE_CLIENT_2_BSS : ALIGN(32)
    {
        start_of_TFM_SP_SECURE_CLIENT_2_DATA = .;
        *tfm_*partition_secure_client_2.*(.bss*)
        *tfm_*partition_secure_client_2.*(COMMON)
        *(TFM_SP_SECURE_CLIENT_2_APP-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_SECURE_CLIENT_2_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_SECURE_CLIENT_2_DATA$$ZI$$Base = ADDR(.TFM_SP_SECURE_CLIENT_2_BSS);
    Image$$ER_TFM_SP_SECURE_CLIENT_2_DATA$$ZI$$Limit = ADDR(.TFM_SP_SECURE_CLIENT_2_BSS) + SIZEOF(.TFM_SP_SECURE_CLIENT_2_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_SECURE_CLIENT_2_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */

#ifdef TFM_PARTITION_APP_ROT

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_APP_ROT_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_APP_ROT_DATA : ALIGN(32)
    {
        *app_rot*.o(.data*)
        *(TFM_APP_ROT_APP-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$.ER_TFM_APP_ROT_DATA$$RW$$Base = ADDR(.ER_TFM_APP_ROT_DATA);
    Image$$.ER_TFM_APP_ROT_DATA$$RW$$Limit = ADDR(.ER_TFM_APP_ROT_DATA) + SIZEOF(.ER_TFM_APP_ROT_DATA);

    .TFM_APP_ROT_BSS : ALIGN(32)
    {
        start_of_TFM_APP_ROT_DATA = .;
        *app_rot*.o(.bss*)
        *app_rot*.o(COMMON)
        *(TFM_APP_ROT_APP-ROT_ATTR_ZI)
        . += (. - start_of_TFM_APP_ROT_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_APP_ROT_DATA$$ZI$$Base = ADDR(.TFM_APP_ROT_BSS);
    Image$$ER_TFM_APP_ROT_DATA$$ZI$$Limit = ADDR(.TFM_APP_ROT_BSS) + SIZEOF(.TFM_APP_ROT_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_APP_ROT_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_APP_ROT */

#ifdef TFM_PARTITION_FFM11

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_FFM11_PRIVATE_DATA_START$$Base = .;

    .ER_TFM_SP_FFM11_DATA : ALIGN(32)
    {
        *tfm_*partition_ffm11.*(.data*)
        *(TFM_SP_FFM11_APP-ROT_ATTR_RW)
        . = ALIGN(32);
    } > RAM AT> FLASH
    Image$$.ER_TFM_SP_FFM11_DATA$$RW$$Base = ADDR(.ER_TFM_SP_FFM11_DATA);
    Image$$.ER_TFM_SP_FFM11_DATA$$RW$$Limit = ADDR(.ER_TFM_SP_FFM11_DATA) + SIZEOF(.ER_TFM_SP_FFM11_DATA);

    .TFM_SP_FFM11_BSS : ALIGN(32)
    {
        start_of_TFM_SP_FFM11_DATA = .;
        *tfm_*partition_ffm11.*(.bss*)
        *tfm_*partition_ffm11.*(COMMON)
        *(TFM_SP_FFM11_APP-ROT_ATTR_ZI)
        . += (. - start_of_TFM_SP_FFM11_DATA) ? 0 : 4;
        . = ALIGN(32);
    } > RAM AT> RAM
    Image$$ER_TFM_SP_FFM11_DATA$$ZI$$Base = ADDR(.TFM_SP_FFM11_BSS);
    Image$$ER_TFM_SP_FFM11_DATA$$ZI$$Limit = ADDR(.TFM_SP_FFM11_BSS) + SIZEOF(.TFM_SP_FFM11_BSS);

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_TFM_SP_FFM11_PRIVATE_DATA_END$$Base = .;

#endif /* TFM_PARTITION_FFM11 */

    /**** APPLICATION RoT DATA end here */

    /* Position tag */
    . = ALIGN(32);
    Image$$PT_APP_DATA_END$$Base = .;
    .TFM_APP_RW_STACK_END : ALIGN(GTZC_RAM_ALIGN)
    {
        . += 0x0;
    } > RAM

    /**** APPLICATION RoT DATA end here */
    Image$$TFM_APP_RW_STACK_END$$Base =  ADDR(.TFM_APP_RW_STACK_END);


#ifdef TFM_PARTITION_TEST_SECURE_SERVICES
    .TFM_SP_SECURE_TEST_PARTITION_LINKER_DATA : ALIGN(32)
    {
        *libc_nano*:*(.data*)
        . = ALIGN(32);
    } > RAM AT> FLASH

    .TFM_SP_SECURE_TEST_PARTITION_LINKER_BSS : ALIGN(32)
    {
        /* FixMe:
         * Test framework use printf to print out test result. Implementation of
         * printf in GCC libc use static data and heap as well. To be able to
         * execute test suites with TFM_LVL=3 this workaround is needed to
         * allocate libc static data and heap within the data section of secure
         * test partition. This can be removed if test service will be executed
         * in privileged mode.
         */
        *libc_nano*:*(.bss*)
        *libc_nano*:*(COMMON)

        __end__ = .;
        PROVIDE(end = .);
        __HeapBase = .;
        . += __heap_size__;
        __HeapLimit = .;
        __heap_limit = .; /* Add for _sbrk */

        . = ALIGN(32);
    } > RAM AT> RAM
#else /* TFM_PARTITION_TEST_SECURE_SERVICES */
    .heap : ALIGN(8)
    {
        __end__ = .;
        PROVIDE(end = .);
        __HeapBase = .;
        . += __heap_size__;
        __HeapLimit = .;
        __heap_limit = .; /* Add for _sbrk */
    } > RAM AT> RAM
#endif /* TFM_PARTITION_TEST_SECURE_SERVICES */



    /*
     * Place the CMSE Veneers (containing the SG instruction) after the code, in a
     * separate 32 bytes aligned region so that the SAU can programmed to just set
     * this region as Non-Secure Callable.
     */
    .gnu.sgstubs : ALIGN(32)
    {
        *(.gnu.sgstubs*)
        . = ALIGN(32);
    } > VENEERS AT> VENEERS
    Load$$LR$$LR_VENEER$$Base = ADDR(.gnu.sgstubs);
    Load$$LR$$LR_VENEER$$Limit = ADDR(.gnu.sgstubs) + SIZEOF(.gnu.sgstubs);

    Load$$LR$$LR_NS_PARTITION$$Base = NS_PARTITION_START;

#ifdef BL2
#if !defined(MCUBOOT_PRIMARY_ONLY)
    Load$$LR$$LR_SECONDARY_PARTITION$$Base = SECONDARY_PARTITION_START;
#endif /* !defined(MCUBOOT_PRIMARY_ONLY) */
#endif /* BL2 */

    PROVIDE(__stack = Image$$ARM_LIB_STACK$$ZI$$Limit);
    PROVIDE(__StackTop = Image$$ARM_LIB_STACK$$ZI$$Limit);
    PROVIDE(__StackLimit = Image$$ARM_LIB_STACK$$ZI$$Base);
}
