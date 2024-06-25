// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2019 AVNET
 */

#include <asm/arch/sys_proto.h>
#include "mx8m_common.h"

const char* mx8m_get_plat_str(void)
{
	switch (get_cpu_type()) {
	case MXC_CPU_IMX8MM:
		return "imx8mm";
	case MXC_CPU_IMX8MP:
		return "imx8mp";
	}

	return "N/A";
}

const char* mx8m_get_proc_str(void)
{
	switch (get_cpu_type()) {
	case MXC_CPU_IMX8MM:
		return "qc";
	case MXC_CPU_IMX8MP:
		return "qc";
	}
	return "N/A";
}
