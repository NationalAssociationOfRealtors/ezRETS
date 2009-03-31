dnl
dnl Enables a universal build
dnl
dnl Variables:
dnl  my_use_universal
dnl
AC_DEFUN(MY_PROG_UNIVERSAL,
[AC_ARG_ENABLE(universal,
AC_HELP_STRING([--enable-universal],
               [enable universal build on OS X (default: disabled)]),
	       test "X$enableval" != "Xno" && my_enable_universal=yes,
           my_enable_universal=no)

my_use_universal=no
if test "$my_enable_universal" = "yes"; then
   UNIVERSAL_FLAGS="-arch i386 -arch ppc"
   my_use_universal=yes
fi
AC_SUBST(UNIVERSAL_FLAGS)
])

dnl
dnl Tests for ccache and modifies CC and CXX appropriately
dnl
dnl Variables:
dnl  my_use_ccache=(yes|no)
dnl
AC_DEFUN(MY_PROG_CCACHE,
[AC_ARG_ENABLE(ccache,
AC_HELP_STRING([--enable-ccache],
               [enable ccache (default: disabled)]),
               test "X$enableval" != "Xno" && my_enable_ccache=yes,
           my_enable_ccache=no)

my_use_ccache=no
if test "$my_enable_ccache" = "yes"; then
  AC_CHECK_PROG(my_have_ccache, ccache, yes, no)
  if test "$my_have_ccache" == "yes"; then
     CC="ccache $CC"
     CXX="ccache $CXX"
     my_use_ccache=yes
   fi
fi
])

dnl
dnl Enable dependency checking
dnl
dnl Variables:
dnl  my_use_depends=(yes|no)
dnl
AC_DEFUN(MY_TEST_DEPENDS,
[AC_ARG_ENABLE(depends,
AC_HELP_STRING([--enable-depends],
               [enable dependency checking (default: disabled)]),
               test "X$enableval" != "Xno" && my_enable_depends=yes,
           my_enable_depends=no)

my_use_depends=no
USE_DEPENDS=0
if test "$my_enable_depends" = "yes"; then
  if test "$my_have_perl" = "yes"; then
    USE_DEPENDS=1
    my_use_depends=yes
  else
    AC_MSG_ERROR([*** Dependecy checking requires Perl ***])
  fi
fi
AC_SUBST(USE_DEPENDS)
])

dnl
dnl Configure librets via librets-config
dnl
AC_DEFUN([MY_TEST_LIBRETS], [
  AC_ARG_WITH(
    librets-config,
    AC_HELP_STRING(
      [--with-librets-config=FILE],
      [find librets configuration information by using FILE as the
       librets-config binary.  By default, we'll look in your PATH.
      ]
    ),
    [
      AC_MSG_CHECKING(for librets-config)
      if test "$withval" != "yes"; then
        my_librets_config=$withval;
        $my_librets_config --cflags >/dev/null 2>&1
        if test "$?" != "0"; then
          AC_MSG_RESULT([unable to successfully run $my_librets_config])
          AC_MSG_ERROR([Aborting])
        else
          AC_MSG_RESULT($my_librets_config)
        fi
      else
        AC_MSG_RESULT()
        AC_MSG_ERROR([You must specify an argument for --with-librets-config])
      fi
    ],
    [
      AC_CHECK_PROG(my_librets_config, librets-config, librets-config, no)
      if test "$my_librets_config" = "no"; then
        AC_MSG_ERROR([librets-config not found])
      fi
    ]
  )

  check="1.3.1"
  check_hex="010301"
  AC_MSG_CHECKING([libRETS version >= $check])
  ver=`$my_librets_config --version`
  hex_ver=`$my_librets_config --vernum`

  ok=`perl -e "print (hex('$hex_ver')>=hex('$check_hex') ? '1' : '0')"`
  if test x$ok == x0; then
     AC_MSG_ERROR([libRETS $ver is too old.  $check or newer is required.])
  fi				       			       
  AC_MSG_RESULT($ver)

  AC_MSG_CHECKING(if libRETS has SQL compiler)
  my_librets_config_sql=`$my_librets_config --sql`
  if test "x$my_librets_config_sql" != "xtrue"; then
     AC_MSG_RESULT(no)
     AC_MSG_ERROR([libRETS must be compiled with --enable-sql-compiler])
  else
     AC_MSG_RESULT(yes)
  fi

  LIBRETS_CFLAGS=`$my_librets_config --cflags`
  LIBRETS_LDFLAGS=`$my_librets_config --libs`
  AC_SUBST(LIBRETS_CFLAGS)
  AC_SUBST(LIBRETS_LDFLAGS)
])

dnl
dnl Configure Boost
dnl
AC_DEFUN([MY_TEST_BOOST], [
  AC_ARG_WITH(
    [boost-prefix],
    AC_HELP_STRING(
      [--with-boost-prefix=PATH],
      [find the Boost headers and libraries in `PATH/include` and  `PATH/lib`.
       By default, checks in /usr and /usr/local.
      ]),
    boost_prefixes="$withval",
	  boost_prefixes="/usr/local /usr")

  if test x"$with_boost" == "xno"; then
	 AC_MSG_ERROR([Boost is required to build librets])
  fi

  for boost_prefix in $boost_prefixes
  do
    boost_version_h="${boost_prefix}/include/boost/version.hpp"
    AC_CHECK_FILE([$boost_version_h], [my_boost_version_h=$boost_version_h])
    test -n "$my_boost_version_h" && break
  done

  if test -z "$my_boost_version_h"; then
    HAVE_BOOST=0
  else
    HAVE_BOOST=1
    BOOST_PREFIX=$boost_prefix
    BOOST_CFLAGS="-I${BOOST_PREFIX}/include"
    BOOST_LIBS=

    check="1_33_0"
    check_int=103300
    AC_MSG_CHECKING([for boost >= $check])
		
    ver=`perl -ane "print /\#define\s+BOOST_LIB_VERSION\s+\"(\S+)\"/" ${BOOST_PREFIX}/include/boost/version.hpp`
		int_ver=`perl -ane "print /\#define\s+BOOST_VERSION\s+(\S+)/" ${BOOST_PREFIX}/include/boost/version.hpp`
    ok=`perl -e "print (($int_ver>=$check_int) ? '1' : '0')"`
    if test x$ok != x0; then
    	my_cv_boost_vers="$ver"
      AC_MSG_RESULT([$my_cv_boost_vers])
    else
      AC_MSG_RESULT(FAILED)
      AC_MSG_ERROR([$ver is too old. Need version $check or higher.])
    fi

dnl    my_lib="${BOOST_PREFIX}/lib/libboost_filesystem.a"
dnl    AC_CHECK_FILE([$my_lib], [BOOST_FILESYSTEM=$my_lib])
dnl    my_lib="${BOOST_PREFIX}/lib/libboost_program_options.a"
dnl    AC_CHECK_FILE([$my_lib], [BOOST_PROGRAM_OPTIONS=$my_lib])
  fi

  AC_SUBST(HAVE_BOOST)
  AC_SUBST(BOOST_PREFIX)
  AC_SUBST(BOOST_CFLAGS)
  AC_SUBST(BOOST_LIBS)
dnl  AC_SUBST(BOOST_FILESYSTEM)
dnl  AC_SUBST(BOOST_PROGRAM_OPTIONS)
])

dnl
dnl Configure iodbc via iodbc-config
dnl
AC_DEFUN([MY_TEST_IODBC], [
  AC_ARG_WITH(
    iodbc-config,
    AC_HELP_STRING(
      [--with-iodbc-config=FILE],
      [find iodbc configuration information by using FILE as the
       iodbc-config binary.  By default, we'll look in your PATH.
      ]
    ),
    [
      AC_MSG_CHECKING(for iodbc-config)
      if test "$withval" != "yes"; then
        my_iodbc_config=$withval;
        $my_iodbc_config --cflags >/dev/null 2>&1
        if test "$?" != "0"; then
          AC_MSG_RESULT([unable to successfully run $my_iodbc_config])
          AC_MSG_ERROR([Aborting])
        else
          AC_MSG_RESULT($my_iodbc_config)
        fi
      else
        AC_MSG_RESULT()
        AC_MSG_ERROR([You must specify an argument for --with-iodbc-config])
      fi
    ],
    [
      AC_CHECK_PROG(my_iodbc_config, iodbc-config, iodbc-config, no)
      if test "$my_iodbc_config" = "no"; then
        HAVE_IODBC=0
      else
        HAVE_IODBC=1
        AC_DEFINE([HAVE_IODBC], [])
        IODBC_CFLAGS=`$my_iodbc_config --cflags`
        IODBC_LDFLAGS=`$my_iodbc_config --libs`
        IODBC_PREFIX=`$my_iodbc_config --prefix`
      fi
    ]
  )

  AC_SUBST(HAVE_IODBC)
  AC_SUBST(IODBC_CFLAGS)
  AC_SUBST(IODBC_LDFLAGS)
  AC_SUBST(IODBC_PREFIX)
])

dnl
dnl Test to see if the known needed bits of unix odbc are around.
dnl if one of these isn't here, configure will stop.
dnl
AC_DEFUN([MY_TEST_UNIXODBC], [
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
])
