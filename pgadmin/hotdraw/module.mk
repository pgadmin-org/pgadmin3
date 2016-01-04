#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/ Makefile fragment
#
#######################################################################

include hotdraw/connectors/module.mk
include hotdraw/figures/module.mk
include hotdraw/handles/module.mk
include hotdraw/locators/module.mk
include hotdraw/main/module.mk
include hotdraw/tools/module.mk
include hotdraw/utilities/module.mk

EXTRA_DIST += \
	hotdraw/module.mk
