AC_PREREQ([2.65])

AC_INIT([clutter-bullet],
        [0.0.0],
        [huaw@mcmaster.ca],
        [clutter-bullet],
        [http://attente.ca/])

AM_INIT_AUTOMAKE

AC_CONFIG_SRCDIR([src/clutter-bullet.h])
AM_CONFIG_HEADER([src/config.h])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL

AC_SUBST([LIBS], [-lm])

PKG_CHECK_MODULES([BULLET], [bullet >= 2.77])
AC_SUBST([BULLET_CFLAGS])
AC_SUBST([BULLET_LIBS])

PKG_CHECK_MODULES([CLUTTER], [clutter-1.0 >= 1.3])
AC_SUBST([CLUTTER_CFLAGS])
AC_SUBST([CLUTTER_LIBS])

# checks for header files
# checks for types
# checks for structures
# checks for compiler characteristics
# checks for library functions
# checks for system services

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
