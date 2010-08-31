AM_CPPFLAGS = $(BULLET_CFLAGS) $(CLUTTER_CFLAGS)
AM_CFLAGS   = -ansi -pedantic -Wall

bin_PROGRAMS = clutter-bullet-test

clutter_bullet_test_SOURCES = clutter-bullet-test.c clutter-bullet-group.cc
clutter_bullet_test_LDADD   = $(LIBS) $(BULLET_LIBS) $(CLUTTER_LIBS)
