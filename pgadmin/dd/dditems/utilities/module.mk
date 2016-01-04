#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/dd/dditems/utilities/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	dd/dditems/utilities/ddPrecisionScaleDialog.cpp \
	dd/dditems/utilities/ddSelectKindFksDialog.cpp \
	dd/dditems/utilities/ddTableNameDialog.cpp
EXTRA_DIST += \
	dd/dditems/utilities/module.mk
