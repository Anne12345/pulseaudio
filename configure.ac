#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE
LT_INIT

AC_PROG_LIBTOOL 

# Checks for programs.
AC_PROG_CC

# Checks for libraries.

AC_ARG_ENABLE([pulse],
  AS_HELP_STRING([--enable-pulse=@<:@yes/auto/no@:>@], [Enable the PulseAudio backend @<:@default=auto@:>@]),
  [],
  [enable_pulse="auto"])
AS_IF([test "x$enable_pulse" != "xno"],
      [PKG_CHECK_MODULES(PULSE, [libpulse libpulse-mainloop-glib libpulse-simple],
         [AC_DEFINE([HAVE_PULSE], 1, [Have PulseAudio support?])
          enable_pulse="yes"],
         [AS_IF([test "x$enable_pulse" = "xyes"],
                AC_MSG_ERROR([PulseAudio requested but not found]))
          enable_pulse="no"
      ])
])
AM_CONDITIONAL([HAVE_PULSE], [test "x$enable_pulse" = "xyes"])

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

AC_OUTPUT([
    Makefile
])
