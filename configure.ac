AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR(project/common/include/ezrets.h)
AC_CONFIG_AUX_DIR(project/build)
AC_CONFIG_HEADERS([project/driver/src/config.h])

dnl Checks for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_CHECK_PROG(my_have_perl, perl, yes, no)
dnl AC_CHECK_PROG(my_have_doxygen, doxygen, yes, no)
VERSION=`perl -ane 'print $1 if /VERSION\s+\=\s+(\S+)/' ${ac_top_builddir}project/build/version.mk`
AC_MSG_CHECKING([ezrets version])
AC_MSG_RESULT($VERSION)
AC_SUBST(VERSION)

MY_PROG_CCACHE
MY_TEST_LIBRETS
MY_TEST_BOOST
if test "$HAVE_BOOST" != "1"; then
   AC_MSG_ERROR([Boost headers are required to build ezRETS])
fi

MY_TEST_IODBC
if test "$HAVE_IODBC" != "1"; then
	AC_CHECK_HEADER([sql.h], [AC_DEFINE([HAVE_SQL_H])],
	 		[AC_MSG_ERROR([Cannot find sql.h])])
	AC_CHECK_HEADER([odbcinst.h], [AC_DEFINE([HAVE_ODBCINST_H])],
	 		[AC_MSG_ERROR([Cannot find odbcinst.h])])
	AC_CHECK_LIB([odbcinst], [SQLWriteDSNToIni],
		     [AC_DEFINE([HAVE_LIBODBCINST])],
		     [AC_MSG_ERROR([Cannot find libodbcinst])])
	AC_CHECK_LIB([odbc], [SQLAllocHandle],
		     [AC_DEFINE([HAVE_LIBODBC])],
		     [AC_MSG_ERROR([Cannot find libodbc])])
fi

AC_CACHE_SAVE

MY_TEST_DEPENDS

AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug],
                             [enable debugging support (default: disabled)]),
               test "X$enableval" != "Xno" && my_enable_debug=yes,
               my_enable_debug=no)

my_compile_type="Normal"
if test -n "$GCC"; then
  if test "$my_enable_debug" = "yes"; then
    CFLAGS="-g"
    LDFLAGS="-g"
    my_compile_type="Debug"
  fi
fi


AC_CONFIG_FILES([
  Makefile
])

AC_OUTPUT

echo
echo Option summary:
echo
echo Use ccache .................: $my_use_ccache
echo Use dependency checking ....: $my_use_depends
echo Compile type................: $my_compile_type
echo
