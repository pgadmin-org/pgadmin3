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
    LIBPQ_HOME=/usr/local
    if test ! -f "${LIBPQ_HOME}/include/libpq-fe.h"
    then
        LIBPQ_HOME=/usr
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
    WX_HOME=/usr/local
    if test ! -f "${WX_HOME}/include/wx/wx.h"
    then
        WX_HOME=/usr
    fi
    ])

    if test -n "${WX_HOME}"
    then
        if test ! -f "${WX_HOME}/bin/wx-config"
        then
            if test ! -f "{WX_HOME}/wx-config"
            then
                AC_MSG_ERROR("Could not find wx-config in ${WX_HOME}")
            else
                WX_CONFIG="${WX_HOME}/wx-config"
            fi
        else
            WX_CONFIG="${WX_HOME}/bin/wx-config"
        fi
        WX_OLD_LDFLAGS=$LDFLAGS
        WX_OLD_CPPFLAGS=$CPPFLAGS
        WX_NEW_LDFLAGS=`${WX_CONFIG} --libs --static`
        if test -f "${WX_HOME}/lib/libstc.a"
        then
            LIBS="$LIBS -lwxxrc -lstc $WX_NEW_LDFLAGS"
        else
            LIBS="$LIBS -lwx_gtkd_stc-2.4 -lwx_gtkd_xrc-2.4 $WX_NEW_LDFLAGS"
        fi
        WX_NEW_CPPFLAGS=`${WX_CONFIG} --cxxflags`
        CPPFLAGS="$CPPFLAGS $WX_NEW_CPPFLAGS"
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
