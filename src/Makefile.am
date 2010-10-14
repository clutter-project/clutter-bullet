AM_CPPFLAGS = $(BULLET_CFLAGS) $(CLUTTER_CFLAGS)
AM_CFLAGS   = # -ansi -pedantic -Wall

CLUTTER_BULLET = clutter-bullet-actor.cc        \
                 clutter-bullet-card.cc         \
                 clutter-bullet-group.cc        \
                 clutter-bullet-joint.cc        \
                 clutter-bullet-motion-state.cc

bin_PROGRAMS = clutter-bullet-test

clutter_bullet_test_SOURCES = clutter-bullet-test.c $(CLUTTER_BULLET)
clutter_bullet_test_LDADD   = $(LIBS) $(BULLET_LIBS) $(CLUTTER_LIBS)
