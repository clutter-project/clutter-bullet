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

#include <btBulletDynamicsCommon.h>

#include "clutter-bullet-private.h"



#define DEG(x) (180 / M_PI * (x))
#define RAD(x) (M_PI / 180 * (x))



ClutterBulletMotionState::ClutterBulletMotionState (ClutterActor *a,
                                                    gdouble       s)
: actor (a),
  scale (s)
{
}



void
ClutterBulletMotionState::getWorldTransform (btTransform &t) const
{
  CoglMatrix    a;
  ClutterVertex x;
  gfloat        w;
  gdouble       rx, ry, rz;

  clutter_actor_get_size (actor, &x.x, &x.y);
  clutter_actor_get_transformation_matrix (actor, &a);

  x.x /= 2;
  x.y /= 2;
  x.z  = 0;
  w    = 1;

  cogl_matrix_transform_point (&a, &x.x, &x.y, &x.z, &w);

  g_object_get (actor, "rotation-angle-x", &rx,
                       "rotation-angle-y", &ry,
                       "rotation-angle-z", &rz, NULL);

  t.setOrigin (btVector3 (x.x, x.y, x.z) / (w * scale));
  t.getBasis ().setEulerZYX (RAD (rx), RAD (ry), RAD (rz));
}



void
ClutterBulletMotionState::setWorldTransform (const btTransform &t)
{
  CoglMatrix    a;
  ClutterVertex x, dx;
  btScalar      rx0, ry0, rz0;
  gdouble       rx1, ry1, rz1;
  gfloat        w, z;

  t.getBasis ().getEulerZYX (rz0, ry0, rx0);

  rx1 = DEG (rx0);
  ry1 = DEG (ry0);
  rz1 = DEG (rz0);

  g_object_set (actor, "rotation-angle-x", rx1,
                       "rotation-angle-y", ry1,
                       "rotation-angle-z", rz1, NULL);

  clutter_actor_get_size (actor, &x.x, &x.y);
  clutter_actor_get_transformation_matrix (actor, &a);

  x.x /= 2;
  x.y /= 2;
  x.z  = 0;
  w    = 1;

  cogl_matrix_transform_point (&a, &x.x, &x.y, &x.z, &w);

  dx.x = scale * t.getOrigin ().x () - x.x / w;
  dx.y = scale * t.getOrigin ().y () - x.y / w;
  dx.z = scale * t.getOrigin ().z () - x.z / w;

  clutter_actor_move_by (actor, dx.x, dx.y);
  clutter_actor_set_depth (actor, clutter_actor_get_depth (actor) + dx.z);
}
