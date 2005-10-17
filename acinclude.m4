#######################################################################
#
# pgAdmin III - PostgreSQL Tools
# $Id$
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
[  --enable-debug       build a debug version of pgAdmin3],
[
if test "$enableval" = yes
then
	pg_debug_build=yes
else
	pg_debug_build=no
fi
],
[pg_debug_build=no])
])
AC_SUBST(pg_debug_build)

############################
# Static build of pgAdmin3 #
############################
AC_DEFUN([ENABLE_STATIC],
[AC_ARG_ENABLE(static,
[  --enable-static      build a statically linked version of pgAdmin3],
[
if test "$enableval" = yes
then
    pg_static_build=yes
    WX_STATIC="--static=yes"
else
    pg_static_build=no
    WX_STATIC="--static=no"
fi
],
[pg_static_build=no
WX_STATIC="--static=no"])
])

############################
# Build an pgAdmin III.app  #
############################
AC_DEFUN([ENABLE_APPBUNDLE],
[AC_ARG_ENABLE(appbundle,
[  --enable-appbundle   Build Mac OS X appbundle],
[
if test "$enableval" = yes
then
    pg_appbundle=yes
    prefix=$(pwd)/tmp
    bundledir="$(pwd)/pgAdmin3.app"
    bindir="$bundledir/Contents/MacOS"
    datadir="$bundledir/Contents/SharedSupport"
    AC_SUBST(bundledir)
else
    pg_appbundle=no
fi
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

    # Solaris needs -lssl for this test
    case "${host}" in
        *solaris*)
            LDFLAGS="$LDFLAGS -L${LIBPQ_HOME}/lib -lssl"
            ;;
        *)
            LDFLAGS="$LDFLAGS -L${LIBPQ_HOME}/lib"
            ;;
    esac


    AC_LANG_SAVE
    AC_LANG_C
    AC_CHECK_LIB(pq, PQexec, [pgsql_cv_libpq=yes], [pgsql_cv_libpq=no])
    if test "$build_cpu-$build_vendor" = "powerpc-apple"; then
        echo -n "Checking if libpq links against libssl: "
        if test "$(otool -L ${LIBPQ_HOME}/lib/libpq.?.dylib | grep -c libssl)" -gt 0
        then
            pgsql_ssl_libpq="yes"
        else
            pgsql_ssl_libpq="no"
        fi
        echo $pgsql_ssl_libpq
    else
        AC_CHECK_LIB(pq, SSL_connect, [pgsql_ssl_libpq=yes], [pgsql_ssl_libpq=no])
    fi
    AC_LANG_RESTORE

    if test "$pgsql_include" != ""
    then
        CPPFLAGS="$CPPFLAGS -I${pgsql_include}"
    else
        CPPFLAGS="$CPPFLAGS -I${LIBPQ_HOME}/include"
    fi

    if test "$pg_static_build" = "yes"
    then
        if test "$build_cpu-$build_vendor" = "powerpc-apple"
        then
            CRYPT_LIB=""
        else
            CRYPT_LIB="-lcrypt"
        fi

        if test "$pgsql_ssl_libpq" = "yes"
        then
            LIBS="${LIBPQ_HOME}/lib/libpq.a $CRYPT_LIB $LIBS -lssl -lcrypto"
        else
            LIBS="${LIBPQ_HOME}/lib/libpq.a $CRYPT_LIB $LIBS -lcrypto"
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
# wxWidgets linking checks                                          #
# This check should be specified last in configure.ac, since all of #
# the above checks affect this test in some way or another.         #
#####################################################################
AC_DEFUN([CHECK_WXWIDGETS],
[AC_MSG_CHECKING(for wxWidgets)
AC_ARG_WITH(wx,
[  --with-wx=DIR       root directory for wxWidgets installation],
[if test "$withval" != no
then
    AC_MSG_RESULT(yes)
    WX_HOME="$withval"
else
    AC_MSG_RESULT(no)
fi], [
    AC_MSG_RESULT(yes)
    WX_HOME=/usr/local/wx2
    if test ! -f "${WX_HOME}/include/wx-2.5/wx/wx.h" -a ! -f "${WX_HOME}/include/wx-2.6/wx/wx.h"
    then
        WX_HOME=/usr/local
        if test ! -f "${WX_HOME}/include/wx-2.5/wx/wx.h" -a ! -f "${WX_HOME}/include/wx-2.6/wx/wx.h"
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
	
	if test "$pg_debug_build" == yes
	then
	    WX_NEW_CPPFLAGS=`${WX_CONFIG} --cppflags --debug=yes`
        CPPFLAGS="$CPPFLAGS $WX_NEW_CPPFLAGS -g -O0"
			
        WX_NEW_LIBS=`${WX_CONFIG} ${WX_STATIC} --libs --debug=yes`
        WX_NEW_CONTRIB_LIBS=`${WX_CONFIG} ${WX_STATIC} --libs stc,ogl --debug=yes`
        LIBS="$LIBS $WX_NEW_LIBS $WX_NEW_CONTRIB_LIBS"
	else
	    WX_NEW_CPPFLAGS=`${WX_CONFIG} --cppflags --debug=no`
        CPPFLAGS="$CPPFLAGS $WX_NEW_CPPFLAGS -O2"
		
        WX_NEW_LIBS=`${WX_CONFIG} ${WX_STATIC} --libs --debug=no`
        WX_NEW_CONTRIB_LIBS=`${WX_CONFIG} ${WX_STATIC} --libs stc,ogl --debug=no`
        LIBS="$LIBS $WX_NEW_LIBS $WX_NEW_CONTRIB_LIBS"
	fi
	
	if test "$WX_NEW_CPPFLAGS" = "" -o "$WX_NEW_LIBS" = "" -o "$WX_NEW_CONTRIB_LIBS" = ""
	then
	    AC_MSG_ERROR([the combination of static/shared and debug/non-debug options selected can not be supported by your wxWidgets installation.])
	fi

    case "${host}" in
        *-apple-darwin*)
            CPPFLAGS="$CPPFLAGS -no-cpp-precomp -fno-rtti"
            ;;
        *solaris*)
            LDFLAGS="$LDFLAGS -lnsl"
            ;;
        *)
            ;;
    esac
fi

# Print a configuration summary
echo
if test "$pg_debug_build" == yes
then
	echo "Building a debug version of pgAdmin: Yes"
else
	echo "Building a debug version of pgAdmin: No"
fi
if test "$pg_static_build" == yes
then
	echo "Statically linking pgAdmin:          Yes"
else
	echo "Statically linking pgAdmin:          No"
fi
if test "$pg_appbundle" == yes
then
	echo "Building a Mac OS X appbundle:       Yes"
else
	echo "Building a Mac OS X appbundle:       No"
fi
echo

# CFLAGS/CXXFLAGS may well contain unwanted settings, so clear them.
CFLAGS=""
CXXFLAGS=""

])
AC_SUBST(WX_CONFIG)
