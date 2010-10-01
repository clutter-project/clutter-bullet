/*
 * Clutter-Bullet
 * Copyright (C) 2010 William Hua
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */



#include <clutter/clutter.h>

#include "clutter-bullet.h"



static ClutterActor *stage;
static ClutterActor *group;



int
main (int   argc,
      char *argv[])
{
  ClutterColor colour[] = {
    { 0, 0, 0, 255 },
    { 255, 0, 0, 255 },
    { 0, 255, 0, 255 },
    { 0, 0, 255, 255 },
    { 255, 255, 0, 255 }
  };

  ClutterActor *actor = NULL;

  clutter_init (&argc, &argv);

  cogl_set_depth_test_enabled (TRUE);

  stage = clutter_stage_get_default ();
  clutter_stage_set_color (CLUTTER_STAGE (stage), colour);
  clutter_actor_show_all (stage);

  group = clutter_bullet_group_new (100);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), group);

  /*
  actor = clutter_rectangle_new_with_color (colour + 1);
  clutter_actor_set_position (actor, 100, 0);
  clutter_actor_set_depth (actor, 10);
  clutter_actor_set_size (actor, 100, 100);
  clutter_actor_set_rotation (actor, CLUTTER_X_AXIS, 20, 50, 50, 0);
  actor = clutter_bullet_card_new (actor, 1);
  clutter_container_add_actor (CLUTTER_CONTAINER (group), actor);
  */

  actor = clutter_rectangle_new_with_color (colour + 2);
  clutter_actor_set_position (actor, 150, 0);
  clutter_actor_set_depth (actor, 0);
  clutter_actor_set_size (actor, 200, 100);
  clutter_actor_set_rotation (actor, CLUTTER_X_AXIS, 10, 100, 50, 0);
  actor = clutter_bullet_card_new (actor, 1);
  clutter_container_add_actor (CLUTTER_CONTAINER (group), actor);

  actor = clutter_rectangle_new_with_color (colour + 3);
  clutter_actor_set_position (actor, 0, 250);
  clutter_actor_set_depth (actor, 0);
  clutter_actor_set_size (actor, 200, 100);
  clutter_actor_set_rotation (actor, CLUTTER_X_AXIS, 90, 100, 50, 0);
  clutter_container_add_actor (CLUTTER_CONTAINER (group), actor);

  actor = clutter_rectangle_new_with_color (colour + 4);
  clutter_actor_set_position (actor, 200, 350);
  clutter_actor_set_depth (actor, 0);
  clutter_actor_set_size (actor, 200, 100);
  clutter_actor_set_rotation (actor, CLUTTER_X_AXIS, 90, 100, 50, 0);
  clutter_container_add_actor (CLUTTER_CONTAINER (group), actor);

  clutter_bullet_group_start (CLUTTER_BULLET_GROUP (group));

  clutter_main ();

  return 0;
}
