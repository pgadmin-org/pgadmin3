###################################################
# Check if release version of PgAdmin3 is desired #
###################################################
AC_DEFUN([ENABLE_RELEASE],
[AC_ARG_ENABLE(release,
[  --enable-release     build release version of PgAdmin3],
[pg_release_build=yes],
[pg_release_build=no])
])

############################################################
# Check to see if a build against wxWindows 2.5 is desired #
############################################################
AC_DEFUN([ENABLE_WX25],
[AC_ARG_ENABLE(wx25,
[  --enable-wx25        build against wxWindows 2.5],
[wx_version=2.5],
[wx_version=2.4])
])

########################################
# Check for PostgreSQL library (libpq) #
########################################
AC_DEFUN([CHECK_LIBPQ],
[AC_MSG_CHECKING(for pgsql)
AC_ARG_WITH(pgsql,
[  --with-pgsql=DIR directory to search for pgsql libraries and headers],
[if test "$withval" != no; then
    AC_MSG_RESULT(yes)
    LIBPQ_HOME="$withval"
else
    AC_MSG_RESULT(no)
fi], [
    AC_MSG_RESULT(yes)
    LIBPQ_HOME=/usr/local/pgsql
    if test ! -f "${LIBPQ_HOME}/include/libpq-fe.h"
    then
        LIBPQ_HOME=/usr/local
        if test ! -f "${LIBPQ_HOME}/include/libpq-fe.h"
        then
            LIBPQ_HOME=/usr
        fi
    fi
])

#
# Locate pgsql
#
if test -n "${LIBPQ_HOME}"
then
    LIBPQ_OLD_LDFLAGS=$LDFLAGS
    LIBPQ_OLD_CPPFLAGS=$CPPFLAGS
    LDFLAGS="$LDFLAGS -L${LIBPQ_HOME}/lib"
    CPPFLAGS="$CPPFLAGS -I${LIBPQ_HOME}/include"
    AC_LANG_SAVE
    AC_LANG_C
    AC_CHECK_LIB(pq, PQexec, [pgsql_cv_libpq=yes], [pgsql_cv_libpq=no])
    AC_CHECK_HEADER(libpq-fe.h, [pgsql_cv_libpqfe_h=yes], [pgsql_cv_libpqfe_h=no])
    AC_LANG_RESTORE
    if test "$pgsql_cv_libpq" = "yes" -a "$pgsql_cv_libpqfe_h" = "yes"
    then
        AC_MSG_CHECKING(pgsql in ${LIBPQ_HOME})
        AC_MSG_RESULT(ok)
        LIBS="$LIBS -lpq"
    else
        AC_MSG_CHECKING(pgsql in ${LIBPQ_HOME})
        LDFLAGS="$LIBPQ_OLD_LDFLAGS"
        CPPFLAGS="$LIBPQ_OLD_CPPFLAGS"
        AC_MSG_RESULT(failed)
        AC_MSG_ERROR(you must specify a valid pgsql installation with --with-pgsql=DIR)
    fi
fi
])

##################################
# Check for wxWindows sub-system #
##################################
AC_DEFUN([CHECK_WXWINDOWS],
[AC_MSG_CHECKING(for wxWindows)
AC_ARG_WITH(wx,
[  --with-wx=DIR root directory for wxWindows installation],
[if test "$withval" != no ; then
    AC_MSG_RESULT(yes)
    WX_HOME="$withval"
else
    AC_MSG_RESULT(no)
fi], [
    AC_MSG_RESULT(yes)
    WX_HOME=/usr/local/wx2
    if test ! -f "${WX_HOME}/include/wx/wx.h"
    then
        WX_HOME=/usr/local
        if test ! -f "${WX_HOME}/include/wx/wx.h"
        then
            WX_HOME=/usr
        fi
    fi
    ])

    if test -n "${WX_HOME}"
    then
        if test ! -f "${WX_HOME}/bin/wx-config"
        then
            if test ! -f "${WX_HOME}/wx-config"
            then
                AC_MSG_ERROR("Could not find wx-config in ${WX_HOME}")
            else
                WX_CONFIG="${WX_HOME}/wx-config"
            fi
        else
            WX_CONFIG="${WX_HOME}/bin/wx-config"
        fi
        LDFLAGS="$LDFLAGS -L${WX_HOME}/lib"
        WX_OLD_LDFLAGS=$LDFLAGS
        WX_OLD_CPPFLAGS=$CPPFLAGS
        if test "$pg_release_build" == "yes"
        then
            WX_NEW_LDFLAGS=`${WX_CONFIG} --libs --shared`
        else
            WX_NEW_LDFLAGS=`${WX_CONFIG} --libs --static`
        fi

        if test "$pg_release_build" == "yes"; then
            if test -f "${WX_HOME}/lib/libstc.so"
            then
                LIBS="$LIBS -lwxxrc -lstc $WX_NEW_LDFLAGS"
            else
                case "${host}" in
                    *-*-linux-*)
                        LIBS="$LIBS -lwx_gtkd_stc-${wx_version} -lwx_gtkd_xrc-${wx_version}"
                        LIBS="$LIBS $WX_NEW_LDFLAGS" ;;
                    *-apple-darwin*)
                        LIBS="$LIBS -lwx_macd_stc-${wx_version} -lwx_macd_xrc-${wx_version}"
                        LIBS="$LIBS $WX_NEW_LDFLAGS"
                        LDFLAGS="$LDFLAGS -flat_namespace" ;;
                    *) ;;
                esac
            fi
        else
            if test -f "${WX_HOME}/lib/libstc.a"
            then
                LIBS="$LIBS -lwxxrc -lstc $WX_NEW_LDFLAGS"
            else
                case "${host}" in
                    *-*-linux-*) 
                        LIBS="$LIBS -lwx_gtkd_stc-${wx_version} -lwx_gtkd_xrc-${wx_version}"
                        LIBS="$LIBS $WX_NEW_LDFLAGS" ;;
                    *-apple-darwin*)
                        LIBS="$LIBS -lwx_macd_stc-${wx_version} -lwx_macd_xrc-${wx_version}"
                        LIBS="$LIBS $WX_NEW_LDFLAGS"
                        LDFLAGS="$LDFLAGS -flat_namespace" ;;
                    *) ;;
                esac
            fi
        fi
        WX_NEW_CPPFLAGS=`${WX_CONFIG} --cxxflags`
        CPPFLAGS="$CPPFLAGS $WX_NEW_CPPFLAGS"
        case "${host}" in
            *-apple-darwin*)
                CPPFLAGS="$CPPFLAGS -no-cpp-precomp -fno-rtti" ;;
            *) ;;
        esac
        AC_LANG_SAVE
        AC_LANG_C
        #AC_CHECK_LIB(what,what,[wx_lib=yes],[wx_lib=no])
        AC_CHECK_HEADER(wx/version.h, [wx_wx_h=yes],[wx_wx_h=no])
        AC_LANG_RESTORE
        #if test "$wx_lib" = "yes" -a "$wx_wx_h" = "yes"
        if test "$wx_wx_h" = "yes"
        then
            AC_MSG_CHECKING(wxWindows in ${WX_HOME})
            AC_MSG_RESULT(ok)
        else
            AC_MSG_CHECKING(wxWindows in ${WX_HOME})
            LDFLAGS="$WX_OLD_LDFLAGS"
            CPPFLAGS="$WX_OLD_CPPFLAGS"
            AC_MSG_RESULT(failed)
            AC_MSG_ERROR(you must specify a valid wxWindows installation with --with-wx=DIR)
        fi
    fi
])
