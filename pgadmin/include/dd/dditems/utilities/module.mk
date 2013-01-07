#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2013, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/dditems/utilities/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/dd/dditems/utilities/ddDataType.h \
	$(srcdir)/include/dd/dditems/utilities/ddPrecisionScaleDialog.h \
	$(srcdir)/include/dd/dditems/utilities/ddSelectKindFksDialog.h \
	$(srcdir)/include/dd/dditems/utilities/ddTableNameDialog.h

EXTRA_DIST += \
	$(srcdir)/include/dd/dditems/utilities/module.mk
