#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2013, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/dditems/locators/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dd/dditems/locators/ddAddColLocator.h \
	$(srcdir)/include/dd/dditems/locators/ddAddFkLocator.h \
	$(srcdir)/include/dd/dditems/locators/ddMinMaxTableLocator.h \
	$(srcdir)/include/dd/dditems/locators/ddRemoveTableLocator.h \
	$(srcdir)/include/dd/dditems/locators/ddScrollBarTableLocator.h \
	$(srcdir)/include/dd/dditems/locators/ddTableBottomLocator.h

EXTRA_DIST += \
	$(srcdir)/include/dd/dditems/locators/module.mk
