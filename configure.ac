AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR(project/common/include/ezrets.h)
AC_CONFIG_AUX_DIR(project/build)
AC_CONFIG_HEADERS([project/common/include/config.h])

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

dnl autoheader wanted this defined, but now that I know how to make
dnl the config.h.in file properly, we don't want this at all.
dnl AC_DEFINE([HAVE_IODBC], [], [Use IODBC for our ODBC support])

MY_PROG_CCACHE
dnl MY_PROG_UNIVERSAL
MY_TEST_LIBRETS
MY_TEST_BOOST
if test "$HAVE_BOOST" != "1"; then
   AC_MSG_ERROR([Boost headers are required to build ezRETS])
fi

MY_TEST_IODBC
if test "$HAVE_IODBC" != "1"; then
   MY_TEST_UNIXODBC
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
