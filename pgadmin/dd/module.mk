#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/ Makefile fragment
#
#######################################################################

include $(srcdir)/dd/dditems/module.mk
include $(srcdir)/dd/ddmodel/module.mk


EXTRA_DIST += \
	$(srcdir)/dd/module.mk
