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

if BUILD_SSH_TUNNEL

pgadmin3_SOURCES += \
		include/libssh2/channel.h \
        include/libssh2/comp.h \
        include/libssh2/crypto.h \
        include/libssh2/libgcrypt.h \
        include/libssh2/libssh2.h \
        include/libssh2/libssh2_priv.h \
        include/libssh2/libssh2_publickey.h \
        include/libssh2/libssh2_sftp.h \
        include/libssh2/mac.h \
        include/libssh2/misc.h \
        include/libssh2/openssl.h \
        include/libssh2/packet.h \
        include/libssh2/session.h \
        include/libssh2/sftp.h \
        include/libssh2/transport.h \
        include/libssh2/userauth.h

EXTRA_DIST += \
	include/libssh2/module.mk \
	include/libssh2/Win32/libssh2_config.h
endif
