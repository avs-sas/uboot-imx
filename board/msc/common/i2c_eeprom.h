#ifndef __MSC_I2C_EEPROM_H__
#define __MSC_I2C_EEPROM_H__
// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2019 AVNET
 */

typedef struct i2c_eeprom {
	unsigned	bus_id;
	unsigned	dev_addr;
	unsigned	alen;
	unsigned	rw_blk_size;
	unsigned	write_delay_ms;
} i2c_eeprom_t;

int i2c_eeprom_read(const i2c_eeprom_t *eeprom, unsigned offset, uchar *buffer, unsigned cnt);
int i2c_eeprom_write(const i2c_eeprom_t *eeprom, unsigned offset, uchar *buffer, unsigned cnt);

#endif /* __MSC_I2C_EEPROM_H__ */
