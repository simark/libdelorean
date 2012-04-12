AC_PREREQ([2.68])
AC_INIT([librbrntrvll], [1.3.3.7])

AM_INIT_AUTOMAKE

AC_CONFIG_MACRO_DIR([config])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_PROG_CXX
LT_INIT

AC_CHECK_HEADERS([stdint.h])
AC_CHECK_FUNCS([memset])


AX_BOOST_BASE([], [], AC_MSG_ERROR(["Boost library was not found."]))
AX_BOOST_THREAD

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.

AC_CONFIG_FILES([
	Makefile \
	include/Makefile \
	src/Makefile \
	])

AC_OUTPUT
