// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2019 AVNET
 */

#include <common.h>
#include <config.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <i2c.h>
#include <command.h>
#include "i2c_eeprom.h"

// #define __DEBUG__

#ifndef CONFIG_SYS_EEPROM_PAGE_WRITE_BITS
	#define CONFIG_SYS_EEPROM_PAGE_WRITE_BITS	8
#endif
#define	EEPROM_PAGE_SIZE	(1 << CONFIG_SYS_EEPROM_PAGE_WRITE_BITS)
#define	EEPROM_PAGE_OFFSET(x)	((x) & (EEPROM_PAGE_SIZE - 1))

__weak int i2c_eeprom_write_enable(const i2c_eeprom_t *eeprom, int state)
{
	return 0;
}

static int i2c_eeprom_addr(const i2c_eeprom_t *eeprom, unsigned offset, uchar *addr)
{
	unsigned blk_off;

	blk_off = offset & 0xff;	/* block offset */

	if (eeprom->alen == 1) {
		addr[0] = offset >> 8;	/* block number */
		addr[1] = blk_off;		/* block offset */
	} else {
		addr[0] = offset >> 16;	/* block number */
		addr[1] = offset >>  8;	/* upper address octet */
		addr[2] = blk_off;	/* lower address octet */
	}

	addr[0] |= eeprom->dev_addr;	/* insert device address */

	return 0;
}

static int i2c_eeprom_len(const i2c_eeprom_t *eeprom, unsigned offset, unsigned end)
{
	unsigned len = end - offset;
	unsigned blk_off = offset & 0xff;
	unsigned maxlen = EEPROM_PAGE_SIZE - EEPROM_PAGE_OFFSET(blk_off);

	if (maxlen > eeprom->rw_blk_size)
		maxlen = eeprom->rw_blk_size;

	if (len > maxlen)
		len = maxlen;

	return len;
}

static int i2c_eeprom_rw_block(const i2c_eeprom_t *eeprom, unsigned offset, uchar *addr,
		uchar *buffer, unsigned len, bool read)
{
	int ret = 0;
	struct udevice *dev;

	ret = i2c_get_chip_for_busnum(eeprom->bus_id, addr[0], eeprom->alen, &dev);
	if (ret) {
		printf("Cannot find udev for bus %d\n", eeprom->bus_id);
		return -ENODEV;
	}

	if (read)
		ret = dm_i2c_read(dev, offset, buffer, len);
	else
		ret = dm_i2c_write(dev, offset, buffer, len);

	if (ret)
		ret = 1;

	return ret;
}

static int i2c_eeprom_rw(const i2c_eeprom_t *eeprom, unsigned offset, uchar *buffer,
	unsigned cnt, bool read)
{
	unsigned end = offset + cnt;
	unsigned len;
	int rcode = 0;
	uchar addr[3];

	while (offset < end) {
		i2c_eeprom_addr(eeprom, offset, addr);
		len = i2c_eeprom_len(eeprom, offset, end);

		rcode = i2c_eeprom_rw_block(eeprom, offset, addr, buffer, len, read);

		buffer += len;
		offset += len;

		if (!read)
			mdelay(eeprom->write_delay_ms);
	}

	return rcode;
}

int i2c_eeprom_read(const i2c_eeprom_t *eeprom, unsigned offset, uchar *buffer, unsigned cnt)
{
#if defined(CONFIG_SYS_I2C)
	if (eeprom->bus_id >= 0)
		i2c_set_bus_num(eeprom->bus_id);
#endif

	/*
	 * Read data until done or would cross a page boundary.
	 * We must write the address again when changing pages
	 * because the next page may be in a different device.
	 */
	return i2c_eeprom_rw(eeprom, offset, buffer, cnt, 1);
}

int i2c_eeprom_write(const i2c_eeprom_t *eeprom, unsigned offset, uchar *buffer, unsigned cnt)
{
	int ret;


#if defined(CONFIG_SYS_I2C)
	if (eeprom->bus_id >= 0)
		i2c_set_bus_num(eeprom->bus_id);
#endif

	i2c_eeprom_write_enable(eeprom, 1);

	/*
	 * Write data until done or would cross a write page boundary.
	 * We must write the address again when changing pages
	 * because the address counter only increments within a page.
	 */
	ret = i2c_eeprom_rw(eeprom, offset, buffer, cnt, 0);

	i2c_eeprom_write_enable(eeprom, 0);

	return ret;
}
