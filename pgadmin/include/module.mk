#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	include/copyright.h \
	include/pgAdmin3.h \
	include/postgres.h \
	include/precomp.h \
	include/svnversion.h \
	include/version.h

EXTRA_DIST += \
    include/module.mk

include include/agent/module.mk
include include/db/module.mk
include include/dd/module.mk
include include/dlg/module.mk
include include/debugger/module.mk
include include/ctl/module.mk
include include/frm/module.mk
include include/images/module.mk
include include/parser/module.mk
include include/pgscript/module.mk
include include/schema/module.mk
include include/slony/module.mk
include include/gqb/module.mk
include include/hotdraw/module.mk
include include/utils/module.mk
include include/ogl/module.mk
include include/libssh2/module.mk

