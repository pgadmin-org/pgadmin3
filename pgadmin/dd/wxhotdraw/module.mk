#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/wxhotdraw/ Makefile fragment
#
#######################################################################

include $(srcdir)/dd/wxhotdraw/connectors/module.mk
include $(srcdir)/dd/wxhotdraw/figures/module.mk
include $(srcdir)/dd/wxhotdraw/handles/module.mk
include $(srcdir)/dd/wxhotdraw/locators/module.mk
include $(srcdir)/dd/wxhotdraw/main/module.mk
include $(srcdir)/dd/wxhotdraw/tools/module.mk
include $(srcdir)/dd/wxhotdraw/utilities/module.mk

EXTRA_DIST += \
	$(srcdir)/dd/wxhotdraw/module.mk
