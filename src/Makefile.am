AM_CPPFLAGS = $(BULLET_CFLAGS) $(CLUTTER_CFLAGS)
AM_CFLAGS   = -ansi -pedantic -Wall

CLUTTER_BULLET = clutter-bullet-actor.cc        \
                 clutter-bullet-card.cc         \
                 clutter-bullet-group.cc        \
                 clutter-bullet-hinge.cc        \
                 clutter-bullet-joint.c         \
                 clutter-bullet-motion-state.cc \
                 clutter-bullet-pivot.cc

lib_LTLIBRARIES = libclutter-bullet.la
noinst_PROGRAMS = clutter-bullet-test

libclutter_bullet_la_SOURCES = $(CLUTTER_BULLET)
libclutter_bullet_la_LIBADD  = $(LIBM) $(BULLET_LIBS) $(CLUTTER_LIBS)

clutter_bullet_test_SOURCES = clutter-bullet-test.c
clutter_bullet_test_LDADD   = -lclutter-bullet
