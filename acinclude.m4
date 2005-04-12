#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# Copyright (C) 2002 - 2005, The pgAdmin Development Team
# This software is released under the Artistic Licence
#
# acinclude.m4 - Build system scripts.
#
#######################################################################

#############################
# Override wx-config binary #
#############################
AC_DEFUN([CHECK_WX_CONFIG_BINARY],
[AC_MSG_CHECKING(for wx-config binary)
AC_ARG_WITH(wx-config,
[  --with-wx-config=FILE  name of wx-config binary],
[if test "$withval" != no; then
    AC_MSG_RESULT(yes)
    wx_config_binary="$withval"
 else
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([you must specify a filename when using --with-wx-config=FILE])
 fi])
 AC_MSG_RESULT(yes)
])

#########################################
# Override PostgreSQL include directory #
#########################################
AC_DEFUN([CHECK_PGSQL_INCLUDE],
[AC_MSG_CHECKING(for pgsql include files)
AC_ARG_WITH(pgsql-include,
[  --with-pgsql-include=DIR  location of PostgreSQL include files],
[if test "$withval" != no; then
    AC_MSG_RESULT(yes)
    pgsql_include="$withval"
 else
    AC_MSG_RESULT(no)
    AC_MSG_ERROR([you must specify a directory when using --with-pgsql-include=DIR])
 fi])
    AC_MSG_RESULT(yes)
])

###########################
# Debug build of pgAdmin3 #
###########################
AC_DEFUN([ENABLE_DEBUG],
[AC_ARG_ENABLE(debug,
[ --enable-debug       build a debug version of pgAdmin3],
[pg_debug_build=yes
CFLAGS="-Wall -g -O0"
CXXFLAGS="-Wall -g -O0"],
[pg_debug_build=no])
])

############################
# Static build of pgAdmin3 #
############################
AC_DEFUN([ENABLE_STATIC],
[AC_ARG_ENABLE(static,
[ --enable-static      build a static version of pgAdmin3],
[pg_static_build=yes],
[pg_static_build=no])
])

############################
# Build an pgAdmin III.app  #
############################
AC_DEFUN([ENABLE_APPBUNDLE],
[AC_ARG_ENABLE(appbundle,
[ --enable-appbundle   Build pgAdmin3.app],
[pg_appbundle=yes
prefix=$(pwd)/tmp
bundledir="$(pwd)/pgAdmin3.app"
bindir="$bundledir/Contents/MacOS"
datadir="$bundledir/Contents/SharedSupport"
AC_SUBST(bundledir)
],
[pg_appbundle=no])
])

########################################
# Check for PostgreSQL library (libpq) #
########################################
AC_DEFUN([CHECK_PGSQL],
[AC_MSG_CHECKING(for pgsql)
AC_ARG_WITH(pgsql,
[  --with-pgsql=DIR    directory to look for pgsql libraries and headers],
[if test "$withval" != no
then
    AC_MSG_RESULT(yes)
    LIBPQ_HOME="$withval"
else
    AC_MSG_RESULT(no)
fi], [
    AC_MSG_RESULT(yes)
    LIBPQ_HOME="/usr/local/pgsql"
    # Check for headers
    if test "$pgsql_include" = ""
    then
        if test ! -f "${LIBPQ_HOME}/include/libpq-fe.h"
        then
            LIBPQ_HOME=/usr/local
            if test ! -f "${LIBPQ_HOME}/include/libpq-fe.h"
            then
                LIBPQ_HOME=/usr
            fi
        fi
        if test -f "/usr/include/libpq-fe.h"
        then
            pgsql_include="/usr/include"
        else
            if test -f "/usr/include/pgsql/libpq-fe.h"
            then
                pgsql_include="/usr/include/pgsql"
            fi
        fi
    fi
])

#
# Check for libpq libraries and headers
#
if test -n "${LIBPQ_HOME}"
then
    PGSQL_OLD_LDFLAGS="$LDFLAGS"
    PGSQL_OLD_CPPFLAGS="$CPPFLAGS"
    LDFLAGS="$LDFLAGS -L${LIBPQ_HOME}/lib"

    AC_LANG_SAVE
    AC_LANG_C
    AC_CHECK_LIB(pq, PQexec, [pgsql_cv_libpq=yes], [pgsql_cv_libpq=no])
    AC_CHECK_LIB(pq, SSL_connect, [pgsql_ssl_libpq=yes], [pgsql_ssl_libpq=no])
    AC_LANG_RESTORE

    if test "$pgsql_include" != ""
    then
        CPPFLAGS="$CPPFLAGS -I${pgsql_include}"
    else
        CPPFLAGS="$CPPFLAGS -I${LIBPQ_HOME}/include"
    fi

    if test "$pg_static_build" = "yes"
    then
        if test "$pgsql_ssl_libpq" = "yes"
        then
            LIBS="${LIBPQ_HOME}/lib/libpq.a -lcrypt $LIBS -lssl -lcrypto"
        else
            LIBS="${LIBPQ_HOME}/lib/libpq.a -lcrypt $LIBS -lcrypto"
        fi
    else
        if test "$pgsql_ssl_libpq" = "yes"
        then
            LIBS="$LIBS -lssl -lcrypto -lpq"
        else
            LIBS="$LIBS -lcrypto -lpq"
        fi
    fi

    AC_LANG_SAVE
    AC_LANG_C
    AC_CHECK_HEADER(libpq-fe.h, [pgsql_cv_libpqfe_h=yes], [pgsql_cv_libpqfe_h=no])
    AC_LANG_RESTORE

    if test "$pgsql_cv_libpq" = "yes" -a "$pgsql_cv_libpqfe_h" = "yes"
    then
        AC_MSG_CHECKING(pgsql in ${LIBPQ_HOME})
        AC_MSG_RESULT(ok)
    else
        AC_MSG_CHECKING(pgsql in ${LIBPQ_HOME})
        AC_MSG_RESULT(failed)
        LDFLAGS="$PGSQL_OLD_LDFLAGS"
        CPPFLAGS="$PGSQL_OLD_CPPFLAGS"
        AC_MSG_ERROR([you must specify a valid pgsql installation with --with-pgsql=DIR])
    fi

    if test "$pgsql_ssl_libpq" = "yes"
    then
        CPPFLAGS="$CPPFLAGS -DSSL"
    fi
fi
])

#####################################################################
# WxWindows linking checks                                          #
# This check should be specified last in configure.ac, since all of #
# the above checks affect this test in some way or another.         #
#####################################################################
AC_DEFUN([CHECK_WXWINDOWS],
[AC_MSG_CHECKING(for wxWindows)
AC_ARG_WITH(wx,
[  --with-wx=DIR       root directory for wxWindows installation],
[if test "$withval" != no
then
    AC_MSG_RESULT(yes)
    WX_HOME="$withval"
else
    AC_MSG_RESULT(no)
fi], [
    AC_MSG_RESULT(yes)
    WX_HOME=/usr/local/wx2
    if test ! -f "${WX_HOME}/include/wx-2.5/wx/wx.h"
    then
        WX_HOME=/usr/local
        if test ! -f "${WX_HOME}/include/wx-2.5/wx/wx.h"
        then
            WX_HOME=/usr
        fi
    fi
])

if test -n "${WX_HOME}"
then
    # Check for wx-config binary
    if test "$wx_config_binary" != ""
    then
        if test ! -f "${WX_HOME}/bin/${wx_config_binary}"
        then
            if test ! -f "${WX_HOME}/${wx_config_binary}"
            then
                AC_MSG_ERROR([Could not find ${wx_config_binary}])
            else
                WX_CONFIG="${WX_HOME}/${wx_config_binary}"
            fi
        else
            WX_CONFIG="${WX_HOME}/bin/${wx_config_binary}"
        fi
    else
        if test ! -f "${WX_HOME}/bin/wx-config"
        then
            if test ! -f "${WX_HOME}/wx-config"
            then
                AC_MSG_ERROR([Could not find wx-config in ${WX_HOME}])
            else
                WX_CONFIG="${WX_HOME}/wx-config"
            fi
        else
            WX_CONFIG="${WX_HOME}/bin/wx-config"
        fi
    fi

    LDFLAGS="$LDFLAGS -L${WX_HOME}/lib"
    WX_OLD_LDFLAGS="$LDFLAGS"
    WX_OLD_CPPFLAGS="$CPPFLAGS"
    if test "$pg_static_build" = "yes"
    then
        WX_NEW_LDFLAGS=`${WX_CONFIG} --static --libs`
    else
        WX_NEW_LDFLAGS=`${WX_CONFIG} --libs`
    fi

    # Which version of wxWindows is this?
    WX_VERSION=`${WX_CONFIG} --version`
    case "${WX_VERSION}" in
        2.5*)
            WX_VERSION="2.5"
            ;;
        2.4*)
            WX_VERSION="2.4"
            ;;
        *)
            ;;
    esac

    # Here we go!!
    if test "$pg_static_build" = "yes"
    then
        case "${WX_NEW_LDFLAGS}" in
            *libwx_mswud-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_mswud_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_mswud_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                LDFLAGS="$LDFLAGS -mwindows -Wl,--subsystem,windows"
                ;;
            *libwx_mswu-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_mswu_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_mswu_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                LDFLAGS="$LDFLAGS -mwindows -Wl,--subsystem,windows"
                ;;
            *libwx_mswd-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_mswd_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_mswd_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                LDFLAGS="$LDFLAGS -mwindows -Wl,--subsystem,windows"
                ;;
            *libwx_msw-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_msw_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_msw_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                LDFLAGS="$LDFLAGS -mwindows -Wl,--subsystem,windows"
                ;;
            *libwx_macud-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_macud_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_macud_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_macud_core*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_macud_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_macud_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_macd-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_macd_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_macd_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_macd_core*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_macd_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_macd_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_mac-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_mac_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_mac_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_mac_core*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_mac_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_mac_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_gtk2ud-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_gtk2ud_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_gtk2ud_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_gtk2ud_core*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_gtk2ud_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_gtk2ud_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_gtk2d-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_gtk2d_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_gtk2d_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_gtk2d_core*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_gtk2d_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_gtk2d_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_gtk2u-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_gtk2u_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_gtk2u_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_gtk2u_core*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_gtk2u_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_gtk2u_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_gtkud-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_gtkud_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_gtkud_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_gtkud_core*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_gtkud_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_gtkud_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_gtkd-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_gtkd_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_gtkd_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_gtkd_core*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_gtkd_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_gtkd_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_gtk-*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_gtk_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_gtk_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *libwx_gtk_core*)
                LIBS="$LIBS ${WX_HOME}/lib/libwx_gtk_stc-${WX_VERSION}.a ${WX_HOME}/lib/libwx_gtk_ogl-${WX_VERSION}.a"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *)
                ;;
        esac
    else
        case "${WX_NEW_LDFLAGS}" in
            *libwx_mswud-*)
                LIBS="$LIBS -lwx_mswud_stc-${WX_VERSION} -lwx_mswud_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                LDFLAGS="$LDFLAGS -mwindows -Wl,--subsystem,windows"
                ;;
            *libwx_mswu-*)
                LIBS="$LIBS -lwx_mswu_stc-${WX_VERSION} -lwx_mswu_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                LDFLAGS="$LDFLAGS -mwindows -Wl,--subsystem,windows"
                ;;
            *libwx_mswd-*)
                LIBS="$LIBS -lwx_mswd_stc-${WX_VERSION} -lwx_mswd_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                LDFLAGS="$LDFLAGS -mwindows -Wl,--subsystem,windows"
                ;;
            *libwx_msw-*)
                LIBS="$LIBS -lwx_msw_stc-${WX_VERSION} -lwx_msw_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                LDFLAGS="$LDFLAGS -mwindows -Wl,--subsystem,windows"
                ;;
            *wx_macud-*)
                LIBS="$LIBS -lwx_macud_stc-${WX_VERSION} -lwx_macud_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_macud_core*)
                LIBS="$LIBS -lwx_macud_stc-${WX_VERSION} -lwx_macud_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_macd-*)
                LIBS="$LIBS -lwx_macd_stc-${WX_VERSION} -lwx_macd_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_macd_core*)
                LIBS="$LIBS -lwx_macd_stc-${WX_VERSION} -lwx_macd_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_mac-*)
                LIBS="$LIBS -lwx_mac_stc-${WX_VERSION} -lwx_mac_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_mac_core*)
                LIBS="$LIBS -lwx_mac_stc-${WX_VERSION} -lwx_mac_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtk2ud-*)
                LIBS="$LIBS -lwx_gtk2ud_stc-${WX_VERSION} -lwx_gtk2ud_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtk2ud_core*)
                LIBS="$LIBS -lwx_gtk2ud_stc-${WX_VERSION} -lwx_gtk2ud_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtk2d-*)
                LIBS="$LIBS -lwx_gtk2d_stc-${WX_VERSION} -lwx_gtk2d_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtk2d_core*)
                LIBS="$LIBS -lwx_gtk2d_stc-${WX_VERSION} -lwx_gtk2d_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtk2u-*)
                LIBS="$LIBS -lwx_gtk2u_stc-${WX_VERSION} -lwx_gtk2u_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtk2u_core*)
                LIBS="$LIBS -lwx_gtk2u_stc-${WX_VERSION} -lwx_gtk2u_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtkud-*)
                LIBS="$LIBS -lwx_gtkud_stc-${WX_VERSION} -lwx_gtkud_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtkud_core*)
                LIBS="$LIBS -lwx_gtkud_stc-${WX_VERSION} -lwx_gtkud_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtkd-*)
                LIBS="$LIBS -lwx_gtkd_stc-${WX_VERSION} -lwx_gtkd_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtkd_core*)
                LIBS="$LIBS -lwx_gtkd_stc-${WX_VERSION} -lwx_gtkd_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtk-*)
                LIBS="$LIBS -lwx_gtk_stc-${WX_VERSION} -lwx_gtk_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtk_core*)
                LIBS="$LIBS -lwx_gtk_stc-${WX_VERSION} -lwx_gtk_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *wx_gtk2_*)
                LIBS="$LIBS -lwx_gtk2_stc-${WX_VERSION} -lwx_gtk2_ogl-${WX_VERSION}"
                LIBS="$LIBS $WX_NEW_LDFLAGS"
                ;;
            *)
                ;;
        esac
    fi

    WX_NEW_CPPFLAGS=`${WX_CONFIG} --cxxflags`
    CPPFLAGS="$CPPFLAGS $WX_NEW_CPPFLAGS -I${WX_HOME}/include/wx-2.5"
    case "${host}" in
        *-apple-darwin*)
            CPPFLAGS="$CPPFLAGS -no-cpp-precomp -fno-rtti"
            ;;
        *)
            ;;
    esac
    wx_wx_h="yes"
    if test ! -f "${WX_HOME}/include/wx-2.5/wx/version.h"
    then
        wx_wx_h="no"
    fi
    if test ! -f "${WX_HOME}/include/wx-2.5/wx/stc/stc.h"
    then
        AC_MSG_ERROR([you need to install the stc package from wxWindows/contrib/src/stc])
        wx_wx_h="no"
    fi
    if test ! -f "${WX_HOME}/include/wx-2.5/wx/ogl/ogl.h"
    then
        AC_MSG_ERROR([you need to install the ogl package from wxWindows/contrib/src/ogl with wxUSE_DEPRECATED=0])
        wx_wx_h="no"
    fi
    if test "$wx_wx_h" = "yes"
    then
        AC_MSG_CHECKING([wxWindows in ${WX_HOME}])
        AC_MSG_RESULT(ok)
    else
        AC_MSG_CHECKING([wxWindows in ${WX_HOME}])
        LDFLAGS="$WX_OLD_LDFLAGS"
        CPPFLAGS="$WX_OLD_CPPFLAGS"
        AC_MSG_RESULT(failed)
        AC_MSG_ERROR([you must specify a valid wxWindows installation with --with-wx=DIR])
    fi
fi
])
AC_SUBST(WX_CONFIG)

