#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# 
# Copyright (C) 2002 - 2016, The pgAdmin Development Team
# This software is released under the PostgreSQL Licence
#
# module.mk - pgadmin/libssh2 Makefile fragment
#
#######################################################################

if BUILD_SSH_TUNNEL

pgadmin3_SOURCES += \
	libssh2/agent.c \
	libssh2/channel.c \
    libssh2/comp.c \
    libssh2/crypt.c \
    libssh2/global.c \
    libssh2/hostkey.c \
    libssh2/keepalive.c \
    libssh2/kex.c \
    libssh2/knownhost.c \
    libssh2/libgcrypt.c \
    libssh2/mac.c \
    libssh2/misc.c \
    libssh2/openssl.c \
    libssh2/packet.c \
    libssh2/pem.c \
    libssh2/publickey.c \
    libssh2/scp.c \
    libssh2/session.c \
    libssh2/sftp.c \
    libssh2/transport.c \
    libssh2/userauth.c \
    libssh2/version.c

EXTRA_DIST += \
	libssh2/module.mk
endif
