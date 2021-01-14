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

#include <rtdevice.h>
#include <of_mtd.h>

struct of_mtd_node
{
    struct rt_mtd_nor_device device;
    of_mtd_t mtd;
};

static rt_size_t _nor_dev_read(struct rt_mtd_nor_device* device, rt_off_t offset, rt_uint8_t* data, rt_uint32_t length)
{
    struct of_mtd_node *of_mtd = (struct of_mtd_node*)device;

    RT_ASSERT(of_mtd != NULL);

    return of_mtd_read(of_mtd->mtd, offset, data, length);
}

static rt_size_t _nor_dev_write(struct rt_mtd_nor_device* device, rt_off_t offset, const rt_uint8_t* data, rt_uint32_t length)
{
    struct of_mtd_node *of_mtd = (struct of_mtd_node*)device;

    RT_ASSERT(of_mtd != NULL);

    return of_mtd_write(of_mtd->mtd, offset, data, length);
}

static rt_err_t _nor_dev_erase(struct rt_mtd_nor_device* device, rt_off_t offset, rt_uint32_t length)
{
    struct of_mtd_node *of_mtd = (struct of_mtd_node*)device;

    RT_ASSERT(of_mtd != NULL);

    return of_mtd_erase(of_mtd->mtd, offset, length);
}

static const struct rt_mtd_nor_driver_ops _nor_ops =
{
    NULL,
    _nor_dev_read,
    _nor_dev_write,
    _nor_dev_erase,
};

static of_mtd_t _part_alloc(const of_mtd_t master, const struct of_mtd_part_info *partinfo)
{
    of_mtd_t part;

    part = rt_malloc(sizeof(struct of_mtd_info));
    if (part)
    {
        part->master = master;
        part->ops = master->ops;
        part->offset = partinfo->offset;
        part->size = partinfo->size;
    }

    return part;
}

static void _part_free(of_mtd_t part)
{
    rt_free(part);
}

of_mtd_t of_mtd_get(const char *name)
{
    struct of_mtd_node *mtd_nor_node;

    mtd_nor_node = (struct of_mtd_node*)rt_device_find(name);
    if (mtd_nor_node)
    {
        return mtd_nor_node->mtd;
    }

    return NULL;
}

/*
 * Register MTD driver
 *
 * @return error code
 *
*/
int of_mtd_register(const char *name, of_mtd_t mtd)
{
    struct of_mtd_node *mtd_nor_node;

    if (of_mtd_get(name) != NULL)
    {
        return -1;
    }

    mtd_nor_node = (struct of_mtd_node*)rt_malloc(sizeof(struct of_mtd_node));
    if (mtd_nor_node)
    {
        mtd_nor_node->mtd = mtd;
        mtd_nor_node->device.block_start = 0;
        mtd_nor_node->device.block_end = mtd->size / mtd->block_size;
        mtd_nor_node->device.block_size = mtd->block_size;
        mtd_nor_node->device.ops = &_nor_ops;

        return rt_mtd_nor_register_device(name, &mtd_nor_node->device);
    }
    else
    {
        return -1;
    }
}

int of_mtd_parts_add(of_mtd_t master, const struct of_mtd_part_info *parts, int np)
{
    int i;

    if (!parts || np <= 0) return -1;

    for (i = 0; i < np; i++)
    {
        of_mtd_t part = _part_alloc(master, &parts[i]);
        if (!part) return -1;

        part->master = master;

        if (of_mtd_register(parts->name, part))
        {
            _part_free(part);
            // TODO error info
            return -1;
        }
    }

    return 0;
}
