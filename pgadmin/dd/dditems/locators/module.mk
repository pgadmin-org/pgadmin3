#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2012, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/dditems/locators/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/dditems/locators/ddAddColLocator.cpp \
	$(srcdir)/dd/dditems/locators/ddAddFkLocator.cpp \
	$(srcdir)/dd/dditems/locators/ddMinMaxTableLocator.cpp \
	$(srcdir)/dd/dditems/locators/ddRemoveTableLocator.cpp \
	$(srcdir)/dd/dditems/locators/ddScrollBarTableLocator.cpp \
	$(srcdir)/dd/dditems/locators/ddTableBottomLocator.cpp

EXTRA_DIST += \
	$(srcdir)/dd/dditems/locators/module.mk
