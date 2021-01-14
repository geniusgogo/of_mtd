/*
 * Copyright (c) 2021-2031, OF(OpenFree) Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-01-14     geniusgogo   first version
 *
 */

#ifndef _OF_NOR_H_
#define _OF_NOR_H_

#include <of_mtd.h>

struct of_nor_ops;

struct of_nor
{
    struct of_mtd_info parent;
    const struct of_nor_ops *ops;
};
typedef struct of_nor* of_nor_t;

struct of_nor_ops
{
    int (*erase)(of_nor_t nor, loff_t addr, size_t len);                     /* return success erased len or error code */
    int (*read)(of_nor_t nor, loff_t addr, uint8_t *buf, size_t len);        /* return success data size or error code */
    int (*write)(of_nor_t nor, loff_t addr, const uint8_t *buf, size_t len); /* return success data size or error code */
};
typedef struct of_nor_ops* of_nor_ops_t;

int of_mtd_nor_init(struct of_nor* nor, const char* devname, uint32_t block_size, uint32_t block_count);

#endif
