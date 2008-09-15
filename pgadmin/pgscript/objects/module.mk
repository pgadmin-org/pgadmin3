#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: pgadmin.sh,v 1.6 2008/07/27 16:25:05 pgunittest Exp $
# Copyright (C) 2002 - 2008, The pgAdmin Development Team
# This software is released under the Artistic Licence
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

