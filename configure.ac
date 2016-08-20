AC_INIT([libashe], [0.0.0], [ashe.goulding@gmail.com])

# Remove default compiler options to use -O3.
: ${CFLAGS=""}
: ${CXXFLAGS=""}

# AM_INIT_AUTOMAKE([1.0 subdir-objects])
AM_INIT_AUTOMAKE([1.0])
LT_INIT

AC_CANONICAL_HOST

AC_CONFIG_MACRO_DIR([m4])

AC_LANG([C++])
LT_LANG([C++])

AC_PROG_CXX

AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [enable debugging, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

os_code=""
case "$host" in
*linux*) os_code="0x02" ;;
*mingw*) os_code="0x10" ;;
*) AC_MSG_ERROR([Could not determine OS code.]) ;;
esac
AC_DEFINE_UNQUOTED([ASHE_HOST_OSCODE], [$os_code])
AC_MSG_NOTICE([Ashe OS Code: $os_code])

PKG_CHECK_MODULES([DEP_OPENSSL], [openssl])

AC_CONFIG_FILES([Makefile src/Makefile examples/Makefile doc/Makefile src/libashe.pc])
AC_OUTPUT
