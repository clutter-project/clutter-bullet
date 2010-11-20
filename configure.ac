AC_PREREQ([2.65])
LT_PREREQ([2.2])

AC_INIT([clutter-bullet],
        [0.0.0],
        [huaw@mcmaster.ca],
        [clutter-bullet],
        [http://attente.ca/])

AM_INIT_AUTOMAKE

LT_INIT

AC_CONFIG_SRCDIR([src/clutter-bullet.h])
AM_CONFIG_HEADER([src/config.h])
AC_CONFIG_MACRO_DIR(m4)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

LT_LIB_M

AC_SUBST([BULLET_VERSION], [2.77])
PKG_CHECK_MODULES([BULLET], [bullet >= $BULLET_VERSION])
AC_SUBST([BULLET_CFLAGS])
AC_SUBST([BULLET_LIBS])

AC_SUBST([CLUTTER_VERSION], [1.3])
AC_SUBST([CLUTTER_API_VERSION], [1.0])
PKG_CHECK_MODULES([CLUTTER], [clutter-$CLUTTER_API_VERSION >= $CLUTTER_VERSION])
AC_SUBST([CLUTTER_CFLAGS])
AC_SUBST([CLUTTER_LIBS])

GTK_DOC_CHECK([1.14], [--flavour no-tmpl])

# checks for header files
# checks for types
# checks for structures
# checks for compiler characteristics
# checks for library functions
# checks for system services

AC_CONFIG_FILES([
  clutter-bullet.pc
  Makefile
  docs/Makefile
  docs/reference/Makefile
  src/Makefile
])

AC_OUTPUT
