#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/ Makefile fragment
#
#######################################################################

include include/hotdraw/connectors/module.mk
include include/hotdraw/figures/module.mk
include include/hotdraw/handles/module.mk
include include/hotdraw/locators/module.mk
include include/hotdraw/main/module.mk
include include/hotdraw/tools/module.mk
include include/hotdraw/utilities/module.mk

EXTRA_DIST += \
	include/hotdraw/module.mk
