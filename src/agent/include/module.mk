#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
# Copyright (C) 2002 - 2006, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - src/agent/include Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/agent/include/dlgJob.h \
	$(srcdir)/agent/include/dlgSchedule.h  \
	$(srcdir)/agent/include/dlgStep.h  \
	$(srcdir)/agent/include/pgaJob.h  \
	$(srcdir)/agent/include/pgaSchedule.h  \
	$(srcdir)/agent/include/pgaStep.h

EXTRA_DIST += \
        $(srcdir)/agent/include/module.mk

