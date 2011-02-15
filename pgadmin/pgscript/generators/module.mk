#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/pgscript/generators/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/pgscript/generators/pgsDateGen.cpp \
	$(srcdir)/pgscript/generators/pgsDateTimeGen.cpp \
	$(srcdir)/pgscript/generators/pgsDictionaryGen.cpp \
	$(srcdir)/pgscript/generators/pgsIntegerGen.cpp \
	$(srcdir)/pgscript/generators/pgsNumberGen.cpp \
	$(srcdir)/pgscript/generators/pgsObjectGen.cpp \
	$(srcdir)/pgscript/generators/pgsRealGen.cpp \
	$(srcdir)/pgscript/generators/pgsReferenceGen.cpp \
	$(srcdir)/pgscript/generators/pgsRegexGen.cpp \
	$(srcdir)/pgscript/generators/pgsStringGen.cpp \
	$(srcdir)/pgscript/generators/pgsTimeGen.cpp

EXTRA_DIST += \
	$(srcdir)/pgscript/generators/module.mk

