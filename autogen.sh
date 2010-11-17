#!/bin/sh

CRAP="aclocal.m4
      config.guess
      config.sub
      configure
      depcomp
      INSTALL
      install-sh
      ltmain.sh
      Makefile.in
      missing
      src/config.h.in
      src/Makefile.in"

if [ "$1" = "clean" ]; then
  make maintainer-clean
  rm -rf ${CRAP}
  exit
fi

libtoolize

aclocal
autoconf
autoheader
automake --add-missing

./configure "$@"
