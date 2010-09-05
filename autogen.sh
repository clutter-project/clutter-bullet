#!/bin/sh

CRAP="aclocal.m4
      configure
      depcomp
      INSTALL
      install-sh
      Makefile.in
      missing
      src/config.h.in
      src/Makefile.in"

if [ "$1" = "clean" ]; then
  make maintainer-clean
  rm -rf ${CRAP}
  exit
fi

aclocal
autoconf
autoheader
automake --add-missing

./configure "$@"
