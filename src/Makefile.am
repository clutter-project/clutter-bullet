AM_CPPFLAGS = $(BULLET_CFLAGS) $(CLUTTER_CFLAGS)
AM_CFLAGS   = -ansi -pedantic -Wall

CLUTTER_BULLET_H = clutter-bullet.h       \
                   clutter-bullet-actor.h \
                   clutter-bullet-card.h  \
                   clutter-bullet-group.h \
                   clutter-bullet-hinge.h \
                   clutter-bullet-joint.h \
                   clutter-bullet-pivot.h

CLUTTER_BULLET_C = clutter-bullet-actor.cc        \
                   clutter-bullet-card.cc         \
                   clutter-bullet-group.cc        \
                   clutter-bullet-hinge.cc        \
                   clutter-bullet-joint.c         \
                   clutter-bullet-motion-state.cc \
                   clutter-bullet-pivot.cc

clutter_bullet_includedir = $(includedir)/clutter-1.0/clutter-bullet
clutter_bullet_include_HEADERS = $(CLUTTER_BULLET_H)

lib_LTLIBRARIES = libclutter-bullet.la

libclutter_bullet_la_SOURCES = $(CLUTTER_BULLET_C)
libclutter_bullet_la_LIBADD  = $(LIBM) $(BULLET_LIBS) $(CLUTTER_LIBS)

noinst_PROGRAMS = clutter-bullet-test

clutter_bullet_test_SOURCES = clutter-bullet-test.c
clutter_bullet_test_LDADD   = -lclutter-bullet
