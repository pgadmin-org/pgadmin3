#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2011, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/agent Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/agent/dlgJob.h \
	$(srcdir)/include/agent/dlgSchedule.h  \
	$(srcdir)/include/agent/dlgStep.h  \
	$(srcdir)/include/agent/pgaJob.h  \
	$(srcdir)/include/agent/pgaSchedule.h  \
	$(srcdir)/include/agent/pgaStep.h

EXTRA_DIST += \
        $(srcdir)/include/agent/module.mk

