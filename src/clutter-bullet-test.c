/*
 * Clutter-Bullet
 * Copyright (C) 2010 William Hua
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
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
    { 255, 255, 0, 255 },
    { 255, 0, 255, 255 }
  };

  ClutterActor  *actor[4];
  ClutterVertex  pivot[4];

  clutter_init (&argc, &argv);

  cogl_set_depth_test_enabled (TRUE);

  stage = clutter_stage_get_default ();
  clutter_stage_set_color (CLUTTER_STAGE (stage), colour);
  clutter_actor_show_all (stage);

  group = clutter_bullet_group_new (100);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), group);

  actor[0] = clutter_rectangle_new_with_color (colour + 1);
  clutter_actor_set_position (actor[0], 100, 0);
  clutter_actor_set_depth (actor[0], 10);
  clutter_actor_set_size (actor[0], 100, 100);
  clutter_actor_set_rotation (actor[0], CLUTTER_X_AXIS, 20, 50, 50, 0);
  actor[0] = clutter_bullet_card_new (actor[0]);
  clutter_container_add_actor (CLUTTER_CONTAINER (group), actor[0]);

  actor[0] = clutter_rectangle_new_with_color (colour + 2);
  clutter_actor_set_position (actor[0], 150, 0);
  clutter_actor_set_depth (actor[0], 0);
  clutter_actor_set_size (actor[0], 200, 100);
  clutter_actor_set_rotation (actor[0], CLUTTER_Y_AXIS, 90, 100, 50, 0);
  actor[0] = clutter_bullet_card_new (actor[0]);
  clutter_container_add_actor (CLUTTER_CONTAINER (group), actor[0]);

  actor[0] = clutter_rectangle_new_with_color (colour + 3);
  clutter_actor_set_position (actor[0], 0, 250);
  clutter_actor_set_depth (actor[0], 0);
  clutter_actor_set_size (actor[0], 200, 100);
  clutter_actor_set_rotation (actor[0], CLUTTER_X_AXIS, 90, 100, 50, 0);
  clutter_container_add_actor (CLUTTER_CONTAINER (group), actor[0]);

  actor[0] = clutter_rectangle_new_with_color (colour + 4);
  clutter_actor_set_position (actor[0], 200, 350);
  clutter_actor_set_depth (actor[0], 0);
  clutter_actor_set_size (actor[0], 200, 100);
  clutter_actor_set_rotation (actor[0], CLUTTER_X_AXIS, 90, 100, 50, 0);
  clutter_container_add_actor (CLUTTER_CONTAINER (group), actor[0]);

  actor[1] = clutter_rectangle_new_with_color (colour + 1);
  clutter_actor_set_position (actor[1], 300, 0);
  clutter_actor_set_depth (actor[1], -10);
  clutter_actor_set_size (actor[1], 200, 100);
  clutter_actor_set_rotation (actor[1], CLUTTER_X_AXIS, 0, 100, 50, 0);
  clutter_container_add_actor (CLUTTER_CONTAINER (group), actor[1]);

  actor[2] = clutter_rectangle_new_with_color (colour + 2);
  clutter_actor_set_position (actor[2], 506, 56);
  clutter_actor_set_depth (actor[2], 40);
  clutter_actor_set_size (actor[2], 100, 100);
  clutter_actor_set_rotation (actor[2], CLUTTER_X_AXIS, 90, 50, 50, 0);
  actor[2] = clutter_bullet_card_new (actor[2]);
  clutter_container_add_actor (CLUTTER_CONTAINER (group), actor[2]);

  actor[3] = clutter_rectangle_new_with_color (colour + 5);
  clutter_actor_set_position (actor[3], 356, 300);
  clutter_actor_set_depth (actor[3], 0);
  clutter_actor_set_size (actor[3], 100, 100);
  clutter_actor_set_rotation (actor[3], CLUTTER_X_AXIS, -1, 50, 50, 0);
  clutter_actor_set_rotation (actor[3], CLUTTER_Y_AXIS, 90, 50, 50, 0);
  actor[3] = clutter_bullet_card_new (actor[3]);
  clutter_container_add_actor (CLUTTER_CONTAINER (group), actor[3]);

  pivot[0].x = 103;
  pivot[0].y = 53;
  pivot[0].z = 0;
  pivot[1].x = -53;
  pivot[1].y = 53;
  pivot[1].z = 0;

  clutter_bullet_pivot_new (actor[1], pivot + 0,
                            actor[2], pivot + 1);

  pivot[0].x = 103;
  pivot[0].y = 53;
  pivot[0].z = 0;
  pivot[1].x = 104;
  pivot[1].y = -53;
  pivot[1].z = 0;
  pivot[2].x = -53;
  pivot[2].y = 53;
  pivot[2].z = 0;
  pivot[3].x = 53;
  pivot[3].y = 53;
  pivot[3].z = 0;

  clutter_bullet_hinge_new (actor[0], pivot + 0, pivot + 1,
                            actor[3], pivot + 2, pivot + 3);

  clutter_bullet_group_start (CLUTTER_BULLET_GROUP (group));

  clutter_main ();

  return 0;
}
