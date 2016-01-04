#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/agent Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/agent/dlgJob.h \
	include/agent/dlgSchedule.h  \
	include/agent/dlgStep.h  \
	include/agent/pgaJob.h  \
	include/agent/pgaSchedule.h  \
	include/agent/pgaStep.h

EXTRA_DIST += \
        include/agent/module.mk

