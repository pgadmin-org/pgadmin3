#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/wxhotdraw/ Makefile fragment
#
#######################################################################

include $(srcdir)/include/dd/wxhotdraw/connectors/module.mk
include $(srcdir)/include/dd/wxhotdraw/figures/module.mk
include $(srcdir)/include/dd/wxhotdraw/handles/module.mk
include $(srcdir)/include/dd/wxhotdraw/locators/module.mk
include $(srcdir)/include/dd/wxhotdraw/main/module.mk
include $(srcdir)/include/dd/wxhotdraw/tools/module.mk
include $(srcdir)/include/dd/wxhotdraw/utilities/module.mk

EXTRA_DIST += \
	$(srcdir)/include/dd/wxhotdraw/module.mk
