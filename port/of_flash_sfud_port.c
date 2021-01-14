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
 
#include <sfud.h>
#include <of_nor.h>

#ifdef RT_USING_SFUD
#include <spi_flash_sfud.h>
#endif

#ifndef OF_MTD_NOR_FLASH_DEV_NAME
#define OF_MTD_NOR_FLASH_DEV_NAME             "norflash0"
#endif

static int read(of_nor_t nor, loff_t addr, uint8_t *buf, size_t len);
static int write(of_nor_t nor, loff_t addr, const uint8_t *buf, size_t len);
static int erase(of_nor_t nor, loff_t addr, size_t len);

static sfud_flash_t sfud_dev = NULL;
static struct of_nor _mtd_nor;

static const struct of_mtd_part_info _parts_info[] = {
    {"wifi_image",           0,     512*1024},
    {"bt_image",      512*1024,     512*1024},
    {"download",     1024*1024,  2*1024*1024},
    {"easyflash",  3*1024*1024,  1*1024*1024},
    {"filesystem", 4*1024*1024, 12*1024*1024},
};

int of_flash_sfud_init(void)
{
#ifdef RT_USING_SFUD
    /* RT-Thread RTOS platform */
    sfud_dev = rt_sfud_flash_find_by_dev_name(OF_MTD_NOR_FLASH_DEV_NAME);
#else
    /* bare metal platform */
    extern sfud_flash sfud_norflash0;
    sfud_dev = &sfud_norflash0;
#endif

    if (NULL == sfud_dev)
    {
        return -1;
    }

    const struct of_nor_ops _nor_ops = {erase, read, write};
    _mtd_nor.ops = &_nor_ops;

    int err = of_mtd_nor_init(&_mtd_nor, "mtdnor0", sfud_dev->chip.erase_gran, sfud_dev->chip.capacity / sfud_dev->chip.erase_gran);

    rt_kprintf("of mtd nor init:%d\n", err);

    err = of_mtd_parts_add(&_mtd_nor.parent, _parts_info, sizeof(_parts_info) / sizeof(_parts_info[0]));

    rt_kprintf("of mtd parts add:%d\n", err);

    return 0;
}

static int read(of_nor_t nor, loff_t addr, uint8_t *buf, size_t len)
{
    sfud_read(sfud_dev, addr, len, buf);

    return len;
}

static int write(of_nor_t nor, loff_t addr, const uint8_t *buf, size_t len)
{
    if (sfud_write(sfud_dev, addr, len, buf) != SFUD_SUCCESS)
    {
        return -1;
    }

    return len;
}

static int erase(of_nor_t nor, loff_t addr, size_t len)
{
    if (sfud_erase(sfud_dev, addr, len) != SFUD_SUCCESS)
    {
        return -1;
    }

    return len;
}
