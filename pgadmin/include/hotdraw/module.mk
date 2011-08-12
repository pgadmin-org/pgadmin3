#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/hotdraw/ Makefile fragment
#
#######################################################################

include $(srcdir)/include/hotdraw/connectors/module.mk
include $(srcdir)/include/hotdraw/figures/module.mk
include $(srcdir)/include/hotdraw/handles/module.mk
include $(srcdir)/include/hotdraw/locators/module.mk
include $(srcdir)/include/hotdraw/main/module.mk
include $(srcdir)/include/hotdraw/tools/module.mk
include $(srcdir)/include/hotdraw/utilities/module.mk

EXTRA_DIST += \
	$(srcdir)/include/hotdraw/module.mk
