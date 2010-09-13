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
  ClutterColor bg = { 0, 0, 0, 255 };

  clutter_init (&argc, &argv);

  stage = clutter_stage_get_default ();
  clutter_stage_set_color (CLUTTER_STAGE (stage), &bg);
  clutter_actor_show_all (stage);

  group = clutter_bullet_group_new (100);
  clutter_container_add_actor (CLUTTER_CONTAINER (stage), group);

  clutter_main ();

  return 0;
}
