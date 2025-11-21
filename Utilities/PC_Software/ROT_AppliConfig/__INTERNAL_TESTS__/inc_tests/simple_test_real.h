/*
 * Copyright (c) 2018 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SIMPLE_TEST_H__
#define __SIMPLE_TEST_H__

#define DATA_IMAGE_EN
#define FLASH_ADDRESS_SECURE   0x8010000     /* TEXT TEXT TEXT TEXT */
#define DATA_IMAGE_NUMBER      0x0          /* TEXT TEXT TEXT TEXT */
#define FLASH_S_PARTITION_SIZE          (0x6000) /* 24 KB for S partition */
#define FLASH_NS_PARTITION_SIZE         (0xa0000) /* 640 KB for NS partition */
/*#define MCUBOOT_OVERWRITE_ONLY*/     /* Defined: the FW installation uses ovewrite method.
                                      UnDefined: The FW installation uses swap mode. */
#endif /* __SIMPLE_TEST_H__ */
