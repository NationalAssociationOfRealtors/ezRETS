AC_PREREQ(2.52)
AC_INIT
AC_CONFIG_SRCDIR(project/common/include/ezrets.h)
AC_CONFIG_AUX_DIR(project/build)
AC_CONFIG_HEADERS([project/common/include/config.h])

dnl I think I want to use AC_CANONICAL_TARGET so I can set some gcc
dnl flags like -fPIC and -fpic per platform.  As well as arguments for
dnl linking and the like
AC_CANONICAL_TARGET

dnl Checks for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

AC_ARG_ENABLE(debug,
              AC_HELP_STRING([--enable-debug],
                             [enable debugging support (default: disabled)]),
               test "X$enableval" != "Xno" && my_enable_debug=yes,
               my_enable_debug=no)

my_compile_type="Normal"
if test -n "$GCC"; then
  if test "$my_enable_debug" = "yes"; then
    CFLAGS="-g -O0"
    LDFLAGS="-g -O0"
    my_compile_type="Debug"
  fi
fi

dnl AC_PROG_LIBTOOL
AC_CHECK_SIZEOF(void*)
AC_CHECK_SIZEOF(long)
AC_CHECK_TYPES(long long)
AC_CHECK_PROG(my_have_perl, perl, yes, no)
dnl AC_CHECK_PROG(my_have_doxygen, doxygen, yes, no)
VERSION=`perl -ane 'print $1 if /VERSION\s+\=\s+(\S+)/' ${ac_top_builddir}project/build/version.mk`
AC_MSG_CHECKING([ezrets version])
AC_MSG_RESULT($VERSION)
AC_SUBST(VERSION)

MY_PROG_CCACHE
dnl MY_PROG_UNIVERSAL
MY_TEST_LIBRETS
MY_TEST_BOOST
if test "$HAVE_BOOST" != "1"; then
   AC_MSG_ERROR([Boost headers are required to build ezRETS])
fi

MY_TEST_IODBC
if test "$HAVE_IODBC" == "1"; then
   ODBC_LIBS="$IODBC_LDFLAGS"
   CFLAGS="$CFLAGS $IODBC_CFLAGS"
else
   MY_TEST_UNIXODBC
   if test "$target_cpu" == "x86_64" -a "$ac_cv_sizeof_voidp" == "8"; then
      AC_MSG_NOTICE([Building with BUILD_REAL_64_BIT_MODE])
      CFLAGS="$CFLAGS -DBUILD_REAL_64_BIT_MODE"
   fi
   ODBC_LIBS="-lodbc -lodbcinst"
fi

AC_SUBST(ODBC_LIBS)

MY_TEST_DEPENDS

case $target in
     *-linux*)
	CFLAGS="$CFLAGS -fPIC -fpic"
       ;;
     *-darwin*)
       CFLAGS="$CFLAGS -DMAC -fPIC"
       ;;
esac

AC_CONFIG_FILES([
  Makefile
])

AC_CACHE_SAVE

AC_OUTPUT

echo
echo Option summary:
echo
echo Use ccache .................: $my_use_ccache
echo Use dependency checking ....: $my_use_depends
echo Compile type................: $my_compile_type
echo
