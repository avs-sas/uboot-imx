// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2019 AVNET
 */

#include <linux/types.h>
#include <linux/errno.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <i2c.h>
#include <command.h>
#include <asm/arch/sys_proto.h>
#include "string.h"
#include "boardinfo.h"

static board_info_t board_info;

static bool bi_check_magic(const board_info_t *bi)
{
	if (!bi)
		goto error;

	if (bi->head.magic[0] != 'm' ||
	    bi->head.magic[1] != 's' ||
	    bi->head.magic[2] != 'c' )
		goto error;

	return true;

error:
	BI_DEBUG("Magic check failed. \n");
	return false;
}

static int bi_calc_checksum(const board_info_t *bi, uint16_t *chksum)
{
	int i;
	const unsigned char *ptr;

	if (!bi)
		return -EINVAL;

	if (bi->head.version == BI_VER_1_0) {
		const bi_v1_0_t *body = &BI_GET_BODY(bi, 1, 0);
		ptr = (const unsigned char*)body;
		*chksum = 0;
		for (i = 0; i < sizeof(*body); i++)
			*chksum += ptr[i];
	} else {
		return -EINVAL;
	}

	return 0;
}

static bool bi_check_checksum(const board_info_t *bi)
{
	uint16_t chksum;
	int ret;

	if (!bi)
		goto error;

	ret = bi_calc_checksum(bi, &chksum);
	if (ret)
		goto error;

	if (chksum != bi->head.body_chksum)
		goto error;

	return true;
error:
	BI_DEBUG("Magic check failed. \n");
	return false;
}

static bool bi_ckeck(const board_info_t *bi)
{
	return bi_check_magic(bi) && bi_check_checksum(bi);
}

void bi_print(const board_info_t *bi)
{
	if (!bi)
		return;

	printf("company .......... %s\n", bi_get_company(bi));
	printf("form factor ...... %s\n", bi_get_form_factor(bi));
	printf("platform ......... %s\n", bi_get_platform(bi));
	printf("processor ........ %s\n", bi_get_processor(bi));
	printf("feature .......... %s\n", bi_get_feature(bi));
	printf("serial ........... %s\n", bi_get_serial(bi));
	printf("revision (MES) ... %s\n", bi_get_revision(bi));
	printf("boot count ....... %d\n", bi_get_boot_count(bi));
}

__weak int read_boardinfo(board_info_t *bi)
{
	return -ENODATA;
}

board_info_t *bi_init(void)
{
	int ret;

	memset(&board_info, 0, sizeof(board_info));

	ret = read_boardinfo(&board_info);
	if (ret)
		goto error;

	if (!bi_ckeck(&board_info)) {
		ret = -EINVAL;
		goto error;
	}

	return &board_info;

error:
	memset(&board_info, 0, sizeof(board_info));
	return NULL;
}

__weak int write_boardinfo(board_info_t *bi)
{
	return -ENODATA;
}

int bi_save(board_info_t *bi)
{
	bi_head_t *head;
	uint16_t chksum = 0;

	if (bi == NULL) return -ENODATA;

	head = &bi->head;
	head->magic[0] = 'm';
	head->magic[1] = 's';
	head->magic[2] = 'c';
	head->version  = BI_CALC_VER(1, 0);
	bi_calc_checksum(bi, &chksum);
	head->body_chksum = chksum;
	head->body_off = sizeof(bi_head_t);
	head->body_len = sizeof(bi_v1_0_t);

	return write_boardinfo(bi);
}

const char* bi_get_company(const board_info_t *bi)
{
	if (BI_HAS_FEATURE(bi, COMPANY))
		return BI_GET_BODY(bi, 1, 0).company;
	return "N/A";
}

__weak const char* bi_get_form_factor(const board_info_t *bi)
{
	return "N/A";
}

__weak const char* bi_get_platform(const board_info_t *bi)
{
	return "N/A";
}

__weak const char* bi_get_processor(const board_info_t *bi)
{
	return "N/A";
}

const char* bi_get_feature(const board_info_t *bi)
{
	if (BI_HAS_FEATURE(bi, FEATURE))
		return BI_GET_BODY(bi, 1, 0).feature;
	return "N/A";
}

const char* bi_get_serial(const board_info_t *bi)
{
	if (BI_HAS_FEATURE(bi, SERIAL))
		return BI_GET_BODY(bi, 1, 0).serial_number;
	return "N/A";
}

const char* bi_get_revision(const board_info_t *bi)
{
	if (BI_HAS_FEATURE(bi, REVISION))
		return BI_GET_BODY(bi, 1, 0).revision;
	return "N/A";
}

int bi_inc_boot_count(board_info_t *bi)
{
	BI_GET_BODY(bi, 1 , 0).boot_count += 1;
	return bi_save(bi);
}

uint32_t bi_get_boot_count(const board_info_t *bi)
{
	return BI_GET_BODY(bi, 1, 0).boot_count;
}
