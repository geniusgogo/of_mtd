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

#include <of_nor.h>

static int _nor_erase(of_mtd_t mtd, loff_t addr, size_t len)
{
    of_nor_t nor;

    nor = (of_nor_t)mtd;
    return nor->ops->erase(nor, addr, len);
}

static int _nor_read(of_mtd_t mtd, loff_t from, struct of_mtd_io_desc *desc)
{
    of_nor_t nor;
    int ret;

    nor = (of_nor_t)mtd;
    ret = nor->ops->read(nor, from, desc->datbuf, desc->datlen);
    if (ret > 0)
    {
        desc->datretlen = ret;
        ret = 0;
    }

    return ret;
}

static int _nor_write(of_mtd_t mtd, loff_t to, struct of_mtd_io_desc *desc)
{
    of_nor_t nor;
    int ret;

    nor = (of_nor_t)mtd;
    ret = nor->ops->write(nor, to, desc->datbuf, desc->datlen);
    if (ret > 0)
    {
        desc->datretlen = ret;
        ret = 0;
    }

    return ret;
}

static const struct of_mtd_ops _ops =
{
    _nor_erase,
    _nor_read,
    _nor_write,
    0,
    0
};

int of_mtd_nor_init(struct of_nor* nor, const char* devname, uint32_t block_size, uint32_t block_count)
{
    if (!nor->ops) return -1;
    if (!devname || !devname[0]) return -2;

    nor->parent.ops = &_ops;
    nor->parent.type = OF_MTD_TYPE_NOR;
    nor->parent.block_size = block_size;
    nor->parent.size = block_size * block_count;
    nor->parent.offset = 0;

    return of_mtd_register(devname, &nor->parent);
}
