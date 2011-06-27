#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/dditems/utilities/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/dd/dditems/utilities/ddPrecisionScaleDialog.cpp \
	$(srcdir)/dd/dditems/utilities/ddSelectKindFksDialog.cpp \
	$(srcdir)/dd/dditems/utilities/ddTableNameDialog.cpp
EXTRA_DIST += \
	$(srcdir)/dd/dditems/utilities/module.mk
