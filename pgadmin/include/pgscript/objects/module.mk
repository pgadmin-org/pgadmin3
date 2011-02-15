#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/pgscript/objects/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/pgscript/objects/pgsGenerator.h \
	$(srcdir)/include/pgscript/objects/pgsNumber.h \
	$(srcdir)/include/pgscript/objects/pgsObjects.h \
	$(srcdir)/include/pgscript/objects/pgsRecord.h \
	$(srcdir)/include/pgscript/objects/pgsString.h \
	$(srcdir)/include/pgscript/objects/pgsVariable.h

EXTRA_DIST += \
	$(srcdir)/include/pgscript/objects/module.mk

