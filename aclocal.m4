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
  LIBRETS_CFLAGS=`$my_librets_config --cflags`
  LIBRETS_LDFLAGS=`$my_librets_config --libs`
  AC_SUBST(LIBRETS_CFLAGS)
  AC_SUBST(LIBRETS_LDFLAGS)
])
