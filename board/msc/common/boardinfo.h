// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2019 AVNET
 */

#ifndef __MSC_BOARDINFO_H__
#define __MSC_BOARDINFO_H__

#define BI_VER_MAJ	1
#define BI_VER_MIN	0

#define BI_COMPANY_LEN		3
#define BI_FEATURE_LEN		8
#define BI_SERIAL_LEN		11
#define BI_REVISION_LEN		2

#define BI_COMPANY_BIT		(1<<0)
#define BI_FEATURE_BIT		(1<<1)
#define BI_SERIAL_BIT		(1<<2)
#define BI_REVISION_BIT		(1<<3)

typedef struct bi_head {
	uint8_t		magic[4];
	uint8_t		version;
	uint8_t		v_min;
	uint16_t	body_len;
	uint16_t	body_off;
	uint16_t	body_chksum;
	uint32_t	reserved[2];
} bi_head_t;

typedef struct bi_v1_0 {
	uint32_t	__feature_bits;
	char		company	[BI_COMPANY_LEN + 1];
	char		feature[BI_FEATURE_LEN + 1];
	char		serial_number[BI_SERIAL_LEN + 1];
	char		revision[BI_REVISION_LEN + 1];
	uint32_t	boot_count;
	uint16_t	reserved;
} bi_v1_0_t;

typedef struct board_info {
	bi_head_t		head;
	union {
		bi_v1_0_t	v1_0;
	} body;
} board_info_t;

#define BI_STR "Boardinfo"

#define BI_CALC_VER(MAJ, MIN) \
	((MAJ)<<4 | (MIN))

#define BI_VER_1_0	BI_CALC_VER	(1, 0)

#define BI_HAS_FEATURE(BI, F) \
	((BI)->body.v1_0.__feature_bits & BI_##F##_BIT)

#define BI_ENABLE_FEATURE(BI, F) \
	do { \
		(BI)->body.v1_0.__feature_bits |= BI_##F##_BIT; \
	} \
	while(0);

#define BI_GET_BODY(BI, MAJ, MIN) \
	((BI)->body.v##MAJ##_##MIN)

#define BI_PRINT(format, ...) \
	do { \
		printf("%s: ", BI_STR); \
		printf(format, ## __VA_ARGS__); \
	} \
	while(0);

#if defined(DEBUG)
  #define BI_DEBUG(format, ...) \
	do { \
		printf("%s: ", BI_STR); \
		printf(format, ## __VA_ARGS__); \
	} \
	while(0);
#else /* defined(DEBUG) */
  #define BI_DEBUG(format, ...)
#endif /* defined(DEBUG) */

board_info_t *bi_init(void);

const char* bi_get_company(const board_info_t *bi);
const char* bi_get_form_factor(const board_info_t *bi);
const char* bi_get_platform(const board_info_t *bi);
const char* bi_get_processor(const board_info_t *bi);
const char* bi_get_feature(const board_info_t *bi);
const char* bi_get_serial(const board_info_t *bi);
const char* bi_get_revision(const board_info_t *bi);
uint32_t bi_get_boot_count(const board_info_t *bi);

int bi_inc_boot_count(board_info_t *bi);

void bi_print(const board_info_t *bi);

#if !defined(CONFIG_SPL_BUILD)
	int bi_save(board_info_t *bi);
#endif /* !defined(CONFIG_SPL_BUILD) */

#endif /* __MSC_BOARDINFO_H__ */
