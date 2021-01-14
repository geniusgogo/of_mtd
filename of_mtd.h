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

#ifndef _OF_MTD_H_
#define _OF_MTD_H_

#include <stddef.h>
#include <stdint.h>

#define OF_MTD_TYPE_NOR     1
#define OF_MTD_TYPE_NAND    2

 /**
  * MTD operation modes
  *
  * @MTD_OPM_PLACE_OOB:	OOB data are placed at the given offset (default)
  * @MTD_OPM_AUTO_OOB:	OOB data are automatically placed at the free areas
  * @MTD_OPM_RAW:	    data are transferred as-is, with no error correction;
  */
enum of_mtd_opm
{
    MTD_OPM_PLACE_OOB = 0,
    MTD_OPM_AUTO_OOB = 1,
    MTD_OPM_RAW = 2,
};

#ifndef loff_t
typedef long loff_t;
#endif

struct of_mtd_oob_region
{
    uint8_t offset;
    uint8_t length;
};

struct of_mtd_info
{
    const struct of_mtd_ops *ops;
    struct of_mtd_info *master;

    uint16_t oob_size;
    uint16_t sector_size;   /* Minimal writable flash unit size */
    uint32_t block_size:28; /* Erase size for the device */
    uint32_t type:4;

    size_t size;    /* Total size of the MTD */
    loff_t offset;  /* At which this MTD starts, from the beginning of the MEMORY */
};
typedef struct of_mtd_info* of_mtd_t;

struct of_mtd_io_desc
{
    uint8_t mode;      /* operation mode(enum mtd_opm) */
    uint8_t ooblen;    /* number of oob bytes to write/read */
    uint8_t oobretlen; /* number of oob bytes written/read */
    uint8_t ooboffs;   /* offset in the oob area  */
    uint8_t *oobbuf;

    size_t  datlen;    /* number of data bytes to write/read */
    size_t  datretlen; /* number of data bytes written/read */
    uint8_t *datbuf;   /* if NULL only oob are read/written */
};

struct of_mtd_ops
{
    int(*erase)(of_mtd_t mtd, loff_t addr, size_t len);    /* return 0 if success */
    int(*read) (of_mtd_t mtd, loff_t from, struct of_mtd_io_desc *ops); /* return 0 if success */
    int(*write) (of_mtd_t mtd, loff_t to, struct of_mtd_io_desc *ops);  /* return 0 if success */
    int(*isbad) (of_mtd_t mtd, uint32_t block);    /* return 1 if bad, 0 not bad */
    int(*markbad) (of_mtd_t mtd, uint32_t block);  /* return 0 if success */
};

struct of_mtd_part_info
{
    const char *name;           /* name of the MTD partion */
    loff_t offset;              /* start addr of partion */
    size_t size;                /* size of partion */
};

// user api
int of_mtd_erase(of_mtd_t mtd, loff_t addr, size_t size);
int of_mtd_read(of_mtd_t mtd, loff_t from, uint8_t *buf, size_t len);
int of_mtd_write(of_mtd_t mtd, loff_t to, const uint8_t *buf, size_t len);
int of_mtd_block_erase(of_mtd_t mtd, uint32_t block);
int of_mtd_read_oob(of_mtd_t mtd, loff_t from, struct of_mtd_io_desc *desc);
int of_mtd_write_oob(of_mtd_t mtd, loff_t to, struct of_mtd_io_desc *desc);
int of_mtd_block_markbad(of_mtd_t mtd, uint32_t block);
int of_mtd_block_isbad(of_mtd_t mtd, uint32_t block);

// porting interface
of_mtd_t of_mtd_get(const char *name);
int of_mtd_register(const char *name, of_mtd_t mtd);
int of_mtd_parts_add(of_mtd_t mtd, const struct of_mtd_part_info *parts, int np);

#endif
