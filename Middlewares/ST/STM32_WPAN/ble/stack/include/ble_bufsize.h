/*****************************************************************************
 * @file    ble_bufsize.h
 *
 * @brief   Definition of BLE stack buffers size
 *****************************************************************************
 * @attention
 *
 * Copyright (c) 2018-2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 *****************************************************************************
 */

#ifndef BLE_BUFSIZE_H__
#define BLE_BUFSIZE_H__


/*
 * BLE_DEFAULT_ATT_MTU: minimum MTU value that GATT must support.
 */
#define BLE_DEFAULT_ATT_MTU                  23

/*
 * BLE_DEFAULT_MAX_ATT_SIZE: maximum attribute size.
 */
#define BLE_DEFAULT_MAX_ATT_SIZE            512

/*
 * BLE_PREP_WRITE_X_ATT: compute how many Prepare Write Request are needed to
 * write a characteristic with size 'max_att' when the used ATT_MTU value is
 * equal to BLE_DEFAULT_ATT_MTU (23).
 */
#define BLE_PREP_WRITE_X_ATT(max_att) \
        (DIVC(max_att, BLE_DEFAULT_ATT_MTU - 5) * 2)

/*
 * BLE_DEFAULT_PREP_WRITE_LIST_SIZE: default minimum Prepare Write List size.
 */
#define BLE_DEFAULT_PREP_WRITE_LIST_SIZE \
        BLE_PREP_WRITE_X_ATT(BLE_DEFAULT_MAX_ATT_SIZE)

/*
 * BLE_MEM_BLOCK_X_MTU: compute how many memory blocks are needed to compose
 * an ATT packet with ATT_MTU=mtu.
 */
#define BLE_MEM_BLOCK_SIZE                   32

#if ((BASIC_FEATURES != 0)||(PERIPHERAL_ONLY != 0))
#define BLE_MEM_BLOCK_X_PTX(n_link)           0
#else
#define BLE_MEM_BLOCK_X_PTX(n_link)           (n_link)
#endif

#define BLE_MEM_BLOCK_X_TX(mtu) \
        (DIVC((mtu) + 4U, BLE_MEM_BLOCK_SIZE) + 1)

#define BLE_MEM_BLOCK_X_RX(mtu, n_link) \
        ((DIVC((mtu) + 4U, BLE_MEM_BLOCK_SIZE) + 2U) * (n_link) + 1)

#define BLE_MEM_BLOCK_X_MTU(mtu, n_link) \
        (BLE_MEM_BLOCK_X_TX(mtu) + BLE_MEM_BLOCK_X_PTX(n_link) + \
         BLE_MEM_BLOCK_X_RX(mtu, n_link))

/*
 * BLE_MBLOCKS_SECURE_CONNECTIONS: minimum number of blocks required for
 * secure connections
 */
#define BLE_MBLOCKS_SECURE_CONNECTIONS        4

/*
 * BLE_MBLOCKS_CALC: minimum number of buffers needed by the stack.
 * This is the minimum racomanded value and depends on:
 *  - pw: size of Prepare Write List
 *  - mtu: ATT_MTU size
 *  - n_link: maximum number of simultaneous connections
 */
#define BLE_MBLOCKS_CALC(pw, mtu, n_link) \
        ((pw) + MAX(BLE_MEM_BLOCK_X_MTU(mtu, n_link), \
                    BLE_MBLOCKS_SECURE_CONNECTIONS))

/*
 * BLE_FIXED_BUFFER_SIZE_BYTES:
 * A part of the RAM, is dynamically allocated by initializing all the pointers
 * defined in a global context variable "mem_alloc_ctx_p".
 * This initialization is made in the Dynamic_allocator functions, which
 * assign a portion of RAM given by the external application to the above
 * mentioned "global pointers".
 *
 * The size of this Dynamic RAM is made of 2 main components:
 * - a part that is parameters-dependent (num of links, GATT buffers, ...),
 *   and which value is made explicit by the following macro;
 * - a part, that may be considered "fixed", i.e. independent from the above
 *   mentioned parameters.
*/
#if (PERIPHERAL_ONLY != 0)
#define BLE_FIXED_BUFFER_SIZE_BYTES     4   /* Peripheral Only */
#elif (BASIC_FEATURES != 0)
#define BLE_FIXED_BUFFER_SIZE_BYTES   260   /* Basic Features */
#else
#define BLE_FIXED_BUFFER_SIZE_BYTES   516   /* Full stack / Basic Plus */
#endif

/*
 * BLE_PER_LINK_SIZE_BYTES: additional memory size used per link
 */
#if (PERIPHERAL_ONLY != 0)
#define BLE_PER_LINK_SIZE_BYTES       148   /* Peripheral Only */
#elif (BASIC_FEATURES != 0)
#define BLE_PER_LINK_SIZE_BYTES       176   /* Basic Features */
#else
#define BLE_PER_LINK_SIZE_BYTES       192   /* Full stack / Basic Plus */
#endif

/*
 * BLE_PER_ADD_BEARER_SIZE_BYTES: additional memory size used per add. bearer
 */

#if (BASIC_FEATURES | PERIPHERAL_ONLY | LL_ONLY | LL_ONLY_BASIC)
#define BLE_PER_ADD_BEARER_SIZE_BYTES   0
#else
#define BLE_PER_ADD_BEARER_SIZE_BYTES  40   /* Full stack / Basic Plus */
#endif

/*
 * BLE_TOTAL_BUFFER_SIZE: this macro returns the amount of memory, in bytes,
 * needed for the storage of data structures (except GATT database elements)
 * whose size depends on the number of supported connections.
 *
 * @param n_link: Maximum number of simultaneous connections that the device
 * will support. Valid values are from 1 to 20.
 *
 * @param mblocks_count: Number of memory blocks allocated for packets.
 *
 * @param n_add_bearer: Number of additional EATT bearers
 */
#define BLE_TOTAL_BUFFER_SIZE(n_link, mblocks_count, n_add_bearer) \
        (16 + BLE_FIXED_BUFFER_SIZE_BYTES + \
         (BLE_PER_LINK_SIZE_BYTES * (n_link)) + \
         (BLE_PER_ADD_BEARER_SIZE_BYTES * (n_add_bearer)) + \
         ((BLE_MEM_BLOCK_SIZE + 8) * (mblocks_count)))

/*
 * BLE_TOTAL_BUFFER_SIZE_GATT: this macro returns the amount of memory,
 * in bytes, needed for the storage of GATT database elements.
 *
 * @param num_gatt_attributes: Maximum number of Attributes (i.e. the number
 * of characteristic + the number of characteristic values + the number of
 * descriptors, excluding the services) that can be stored in the GATT
 * database. Note that certain characteristics and relative descriptors are
 * added automatically during device initialization so this parameters should
 * be 9 plus the number of user Attributes
 *
 * @param num_gatt_services: Maximum number of Services that can be stored in
 * the GATT database. Note that the GAP and GATT services are automatically
 * added so this parameter should be 2 plus the number of user services
 *
 * @param att_value_array_size: Size of the storage area for Attribute values.
  */
#define BLE_TOTAL_BUFFER_SIZE_GATT(num_gatt_attributes, num_gatt_services, att_value_array_size) \
        (((((att_value_array_size) - 1) | 3) + 1) + \
         (40 * (num_gatt_attributes)) + (48 * (num_gatt_services)))


#endif /* BLE_BUFSIZE_H__ */
