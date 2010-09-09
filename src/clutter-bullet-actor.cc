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

#include "clutter-bullet-actor.h"



G_DEFINE_INTERFACE (
  ClutterBulletActor,
  clutter_bullet_actor,
  G_TYPE_INVALID
);



static void
clutter_bullet_actor_default_init (ClutterBulletActorInterface *klass)
{
  klass->get_mass  = NULL;
  klass->get_shape = NULL;
}



gdouble
clutter_bullet_actor_get_mass (ClutterBulletActor *self)
{
  return CLUTTER_BULLET_ACTOR_GET_INTERFACE (self)->get_mass (self);
}



btCollisionShape *
clutter_bullet_actor_get_shape (ClutterBulletActor *self)
{
  return CLUTTER_BULLET_ACTOR_GET_INTERFACE (self)->get_shape (self);
}
