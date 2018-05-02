AC_INIT([libashe], [1.0.0], [mieabby@gmail.com])

# Remove default compiler options to use -O3.
: ${CFLAGS=""}
: ${CXXFLAGS=""}

AM_INIT_AUTOMAKE([1.0 subdir-objects])
LT_INIT

AC_CANONICAL_HOST

AC_CONFIG_MACRO_DIR([m4])

AC_LANG([C++])
LT_LANG([C++])

AC_PROG_CXX

AC_ARG_ENABLE(debug,
AS_HELP_STRING([--enable-debug],
               [build with debug flags, default: no]),
[case "${enableval}" in
             yes) debug=true ;;
             no)  debug=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
esac],
[debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")

AC_ARG_ENABLE(tests,
AS_HELP_STRING([--enable-tests],
               [build tests, default: no]),
[case "${enableval}" in
             yes) tests=true ;;
             no)  tests=false ;;
             *)   AC_MSG_ERROR([bad value ${enableval} for --enable-tests]) ;;
esac],
[tests=false])
AM_CONDITIONAL(TESTS, test x"$tests" = x"true")
AM_COND_IF([TESTS], [
    AC_CHECK_HEADERS([gtest/gtest.h],
    [],
    [AC_MSG_ERROR([gtest/gtest.h not found.])])
])

lashe_os_code=""
case "$host" in
*linux*) lashe_os_code="0x02" ;;
*mingw*) lashe_os_code="0x10" ;;
*) AC_MSG_ERROR([Could not determine OS code.]) ;;
esac
AC_DEFINE_UNQUOTED([ASHE_HOST_OSCODE], [$lashe_os_code])
AC_MSG_NOTICE([Determined OS Code: $lashe_os_code])

PKG_CHECK_MODULES([DEP_OPENSSL], [openssl])

AC_CONFIG_FILES([Makefile src/Makefile doc/Makefile src/libashe.pc])
AC_OUTPUT
