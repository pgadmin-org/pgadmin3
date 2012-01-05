#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2012, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/dditems/ Makefile fragment
#
#######################################################################

include $(srcdir)/dd/dditems/figures/module.mk
include $(srcdir)/dd/dditems/handles/module.mk
include $(srcdir)/dd/dditems/locators/module.mk
include $(srcdir)/dd/dditems/tools/module.mk
include $(srcdir)/dd/dditems/utilities/module.mk

EXTRA_DIST += \
	$(srcdir)/dd/dditems/module.mk
