#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2010, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/include/ Makefile fragment
#
#######################################################################

pgadmin3_SOURCES += \
	$(srcdir)/include/ogl/basic.h \
        $(srcdir)/include/ogl/bmpshape.h \
        $(srcdir)/include/ogl/composit.h \
        $(srcdir)/include/ogl/divided.h \
        $(srcdir)/include/ogl/drawnp.h \
        $(srcdir)/include/ogl/linesp.h \
        $(srcdir)/include/ogl/misc.h \
        $(srcdir)/include/ogl/ogl.h \
        $(srcdir)/include/ogl/basicp.h \
        $(srcdir)/include/ogl/canvas.h \
        $(srcdir)/include/ogl/constrnt.h \
        $(srcdir)/include/ogl/drawn.h \
        $(srcdir)/include/ogl/lines.h \
        $(srcdir)/include/ogl/mfutils.h \
        $(srcdir)/include/ogl/ogldiag.h

EXTRA_DIST += \
	$(srcdir)/include/ogl/module.mk \
	$(srcdir)/include/ogl/README

