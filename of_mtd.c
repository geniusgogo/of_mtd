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

#include <of_mtd.h>

int of_mtd_block_erase(of_mtd_t mtd, uint32_t block)
{
    uint32_t total_blks;
    loff_t addr;

    total_blks = mtd->size / mtd->block_size;
    if (block >= total_blks)
        return -1;
    addr = mtd->offset + mtd->block_size * block;

    return mtd->ops->erase(mtd->master, addr, mtd->block_size);
}

int of_mtd_block_isbad(of_mtd_t mtd, uint32_t block)
{
    uint32_t total_blks, offset_blk;

    if (!mtd->ops->isbad)
        return 0;

    total_blks = mtd->size / mtd->block_size;
    if (block >= total_blks)
        return -1;
    offset_blk = mtd->offset / mtd->block_size;

    return mtd->ops->isbad(mtd->master, block + offset_blk);
}

int of_mtd_block_markbad(of_mtd_t mtd, uint32_t block)
{
    uint32_t total_blks, offset_blk;

    if (!mtd->ops->markbad)
        return -1;

    total_blks = mtd->size / mtd->block_size;
    if (block >= total_blks)
        return -1;
    offset_blk = mtd->offset / mtd->block_size;

    return mtd->ops->markbad(mtd->master, block + offset_blk);
}

int of_mtd_erase(of_mtd_t mtd, loff_t addr, size_t size)
{
    if (addr > mtd->size || (addr + size) > mtd->size)
        return -1;
    addr += mtd->offset;

    return mtd->ops->erase(mtd->master, addr, size);
}

/*
* Read data only
*
* @from offset to read from
* @return success size or error code
*/
int of_mtd_read(of_mtd_t mtd, loff_t from, uint8_t *buf, size_t len)
{
    int ret;
    struct of_mtd_io_desc desc = {0};

    if (from < 0 || from >= (loff_t)mtd->size || len > mtd->size - from)
        return -1;
    if (!len)
        return 0;

    desc.datbuf = buf;
    desc.datlen = len;
    ret = mtd->ops->read(mtd->master, from + mtd->offset, &desc);
    if (ret)
        return ret;

    return desc.datretlen;
}

/**
* Write data only
*
* @to offset to write from
* @return success size or error code
*/
int of_mtd_write(of_mtd_t mtd, loff_t to, const uint8_t *buf, size_t len)
{
    int ret;
    struct of_mtd_io_desc desc = {0};

    if (to < 0 || to >= (loff_t)mtd->size || len > mtd->size - to)
        return -1;
    if (!mtd->ops->write)
        return -1;
    if (!len)
        return 0;

    desc.datbuf = (uint8_t*)buf;
    desc.datlen = len;
    ret = mtd->ops->write(mtd->master, to + mtd->offset, &desc);
    if (ret)
        return ret;

    return desc.datretlen;
}

/**
* Read data and/or out-of-band
*
* @from offset to read from
* @desc sector operation description structure
* @return error code, 0 success
*/
int of_mtd_read_oob(of_mtd_t mtd, loff_t from, struct of_mtd_io_desc *desc)
{
    desc->datretlen = 0;
    desc->oobretlen = 0;

    if (from < 0 || from >= (loff_t)mtd->size)
        return -1;

    if (desc->datbuf && (desc->datlen > (mtd->size - from)))
        return -1;

    return mtd->ops->read(mtd->master, from + mtd->offset, desc);
}

/**
* Write data and/or out-of-band
*
* @to offset to read from
* @desc sector operation description structure
* @return error code, 0 success
*/
int of_mtd_write_oob(of_mtd_t mtd, loff_t to, struct of_mtd_io_desc *desc)
{
    desc->datretlen = 0;
    desc->oobretlen = 0;

    if (to < 0 || to >= (loff_t)mtd->size)
        return -1;

    if (desc->datbuf && (desc->datlen >(mtd->size - to)))
        return -1;

    return mtd->ops->write(mtd->master, to + mtd->offset, desc);
}
