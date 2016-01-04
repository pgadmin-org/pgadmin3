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
	include/ogl/basic.h \
        include/ogl/bmpshape.h \
        include/ogl/composit.h \
        include/ogl/divided.h \
        include/ogl/drawnp.h \
        include/ogl/linesp.h \
        include/ogl/misc.h \
        include/ogl/ogl.h \
        include/ogl/basicp.h \
        include/ogl/canvas.h \
        include/ogl/constrnt.h \
        include/ogl/drawn.h \
        include/ogl/lines.h \
        include/ogl/mfutils.h \
        include/ogl/ogldiag.h

EXTRA_DIST += \
	include/ogl/module.mk \
	include/ogl/README

