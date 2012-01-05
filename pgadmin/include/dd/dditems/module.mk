#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2012, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/dditems/ Makefile fragment
#
#######################################################################

include $(srcdir)/include/dd/dditems/figures/module.mk
include $(srcdir)/include/dd/dditems/handles/module.mk
include $(srcdir)/include/dd/dditems/locators/module.mk
include $(srcdir)/include/dd/dditems/tools/module.mk
include $(srcdir)/include/dd/dditems/utilities/module.mk

EXTRA_DIST += \
	$(srcdir)/include/dd/dditems/module.mk
