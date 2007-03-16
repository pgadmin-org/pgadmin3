#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id: module.mk 6067 2007-03-15 15:44:55Z dpage $
# Copyright (C) 2002 - 2007, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# module.mk - debugger/include/images/ Makefile fragment
#
#######################################################################

debugger_SOURCES += \
	$(srcdir)/include/images/bug.xpm \
	$(srcdir)/include/images/clabrk.xpm \
	$(srcdir)/include/images/clearall.xpm \
	$(srcdir)/include/images/clip_copy.xpm \
	$(srcdir)/include/images/clip_cut.xpm \
	$(srcdir)/include/images/clip_paste.xpm \
	$(srcdir)/include/images/clrBreak.xpm \
	$(srcdir)/include/images/clrWatch.xpm \
	$(srcdir)/include/images/continue.xpm \
	$(srcdir)/include/images/execute1.xpm \
	$(srcdir)/include/images/execute.xpm \
	$(srcdir)/include/images/file_open.xpm \
	$(srcdir)/include/images/file_save.xpm \
	$(srcdir)/include/images/img.xpm \
	$(srcdir)/include/images/orange.xpm \
	$(srcdir)/include/images/pgAdmin3.xpm \
	$(srcdir)/include/images/redo.xpm \
	$(srcdir)/include/images/restart.xpm \
	$(srcdir)/include/images/setBreak.xpm \
	$(srcdir)/include/images/setPC.xpm \
	$(srcdir)/include/images/setWatch.xpm \
	$(srcdir)/include/images/stepInto.xpm \
	$(srcdir)/include/images/stepOver.xpm \
	$(srcdir)/include/images/stop.xpm \
	$(srcdir)/include/images/undo.xpm

EXTRA_DIST += \
        $(srcdir)/include/images/module.mk
