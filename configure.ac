#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([fastaextract], [0.01], [edeveaud@pasteur.fr])
AC_CONFIG_SRCDIR([src/faextract.c])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
AC_CHECK_PROG(POD2MAN, pod2man, pod2man, :)

# Checks for libraries.
AC_CHECK_LIB([bioseq], [fasta_parse])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h squizz/sequence.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strerror strpbrk strndup strchr])

AC_CONFIG_FILES([Makefile src/Makefile docs/Makefile tests/Makefile])
AC_OUTPUT
