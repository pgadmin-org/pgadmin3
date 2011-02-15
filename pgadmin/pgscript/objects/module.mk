#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/objects/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/pgscript/objects/pgsGenerator.cpp \
	$(srcdir)/pgscript/objects/pgsNumber.cpp \
	$(srcdir)/pgscript/objects/pgsRecord.cpp \
	$(srcdir)/pgscript/objects/pgsString.cpp \
	$(srcdir)/pgscript/objects/pgsVariable.cpp

EXTRA_DIST += \
	$(srcdir)/pgscript/objects/module.mk

