#!/bin/sh

CRAP="aclocal.m4
      config.guess
      config.sub
      configure
      depcomp
      gtk-doc.make
      INSTALL
      install-sh
      ltmain.sh
      m4
      Makefile.in
      missing
      docs/Makefile.in
      docs/reference/clutter-bullet-docs.sgml
      docs/reference/clutter-bullet-overrides.txt
      docs/reference/clutter-bullet-sections.txt
      docs/reference/clutter-bullet.types
      docs/reference/Makefile.in
      src/config.h.in
      src/Makefile.in"

if [ "$1" = "clean" ]; then
  make maintainer-clean
  rm -rf ${CRAP}
  exit
fi

libtoolize
gtkdocize --flavour no-tmpl

aclocal
autoconf
autoheader
automake --add-missing

./configure "$@"
