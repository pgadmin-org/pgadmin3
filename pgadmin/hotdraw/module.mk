#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/hotdraw/ Makefile fragment
#
#######################################################################

include $(srcdir)/hotdraw/connectors/module.mk
include $(srcdir)/hotdraw/figures/module.mk
include $(srcdir)/hotdraw/handles/module.mk
include $(srcdir)/hotdraw/locators/module.mk
include $(srcdir)/hotdraw/main/module.mk
include $(srcdir)/hotdraw/tools/module.mk
include $(srcdir)/hotdraw/utilities/module.mk

EXTRA_DIST += \
	$(srcdir)/hotdraw/module.mk
