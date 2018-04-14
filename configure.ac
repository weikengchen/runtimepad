#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(runtimepad, 1.0.0, w.k@berkeley.edu)
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

AC_CANONICAL_HOST
case $host_os in
   darwin*)
	AC_MSG_ERROR([OSX is not supported.])
	;;
esac

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(gmp, __gmpz_init, ,
  [AC_MSG_ERROR([GNU MP not found, see https://gmplib.org/])])

# Checks for header files.
AC_CHECK_HEADERS([unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_CHECK_FUNCS([clock_gettime])

AC_OUTPUT(Makefile)
