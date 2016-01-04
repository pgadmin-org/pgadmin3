#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/agent/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
    agent/dlgJob.cpp \
	  agent/dlgSchedule.cpp \
	  agent/dlgStep.cpp \
	  agent/pgaJob.cpp \
	  agent/pgaSchedule.cpp \
	  agent/pgaStep.cpp

EXTRA_DIST += \
    agent/module.mk


