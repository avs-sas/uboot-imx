/*
 * Copyright (C) 2019 AVNET Integrated, MSC Technologies GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation version 2.
 *
 * This program is distributed "as is" WITHOUT ANY WARRANTY of any
 * kind, whether express or implied; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <common.h>
#include <malloc.h>
#include <errno.h>
#include "../common/i2c_eeprom.h"
#include "../common/boardinfo.h"
#include "../common/mx8m_common.h"

i2c_eeprom_t boardinfo_eeprom = {
	.bus_id		= BI_EEPROM_I2C_BUS_ID,
	.dev_addr	= BI_EEPROM_I2C_ADDR,
	.alen		= 2,
	.rw_blk_size	= 16,
	.write_delay_ms = 5,
};

int read_boardinfo(board_info_t *bi)
{
	return i2c_eeprom_read(&boardinfo_eeprom, 0, (uint8_t*)bi, sizeof(*bi));
}

int write_boardinfo(board_info_t *bi)
{
	return i2c_eeprom_write(&boardinfo_eeprom, 0, (uint8_t*)bi, sizeof(*bi));
}

const char* bi_get_form_factor(const board_info_t *bi)
{
	return "sm2s";
}

const char* bi_get_platform(const board_info_t *bi)
{
	return mx8m_get_plat_str();
}

const char* bi_get_processor(const board_info_t *bi)
{
	return mx8m_get_proc_str();
}
