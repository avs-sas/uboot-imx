#
# Copyright (C) 2019 AVNET Integrated, MSC Technologies GmbH
#
# SPDX-License-Identifier:      GPL-2.0+
#

MINIMAL=

ifdef CONFIG_SPL_BUILD
ifdef CONFIG_SPL_INIT_MINIMAL
MINIMAL=y
endif
endif

ifdef MINIMAL
# necessary to create built-in.o
obj- := __dummy__.o
else

obj-y += i2c_eeprom.o
obj-y += mmc.o
obj-y += boardinfo.o
obj-$(CONFIG_IMX8M) += mx8m_common.o
endif
