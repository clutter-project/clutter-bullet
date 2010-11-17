AM_CPPFLAGS = $(BULLET_CFLAGS) $(CLUTTER_CFLAGS)
AM_CFLAGS   = -ansi -pedantic -Wall

CLUTTER_BULLET = clutter-bullet-actor.cc        \
                 clutter-bullet-card.cc         \
                 clutter-bullet-group.cc        \
                 clutter-bullet-hinge.cc        \
                 clutter-bullet-joint.c         \
                 clutter-bullet-motion-state.cc \
                 clutter-bullet-pivot.cc

bin_PROGRAMS = clutter-bullet-test

clutter_bullet_test_SOURCES = clutter-bullet-test.c $(CLUTTER_BULLET)
clutter_bullet_test_LDADD   = $(LIBM) $(BULLET_LIBS) $(CLUTTER_LIBS)
