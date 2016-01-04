#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/dd/dditems/utilities/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/dd/dditems/utilities/ddDataType.h \
	include/dd/dditems/utilities/ddPrecisionScaleDialog.h \
	include/dd/dditems/utilities/ddSelectKindFksDialog.h \
	include/dd/dditems/utilities/ddTableNameDialog.h

EXTRA_DIST += \
	include/dd/dditems/utilities/module.mk
