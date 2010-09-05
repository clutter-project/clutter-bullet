#!/bin/sh

CRAP=""

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
