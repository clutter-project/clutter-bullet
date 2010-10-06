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



#include <cmath>

#include <clutter/clutter.h>

#include <btBulletDynamicsCommon.h>

#include "clutter-bullet-private.h"



#define DEG(x) (180 / M_PI * (x))
#define RAD(x) (M_PI / 180 * (x))



static void clutter_bullet_get_euler_angles (const btMatrix3x3 *a,
                                             gdouble           *x,
                                             gdouble           *y,
                                             gdouble           *z);



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
  gdouble       rx, ry, rz;
  gfloat        w, z;

  clutter_bullet_get_euler_angles (&t.getBasis (), &rx, &ry, &rz);

  g_object_set (actor, "rotation-angle-x", DEG (rx),
                       "rotation-angle-y", DEG (ry),
                       "rotation-angle-z", DEG (rz), NULL);

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



static void
clutter_bullet_get_euler_angles (const btMatrix3x3 *a,
                                 gdouble           *x,
                                 gdouble           *y,
                                 gdouble           *z)
{
  if ((*a)[2][0] <= -1)
  {
    *x = atan2 ((*a)[0][1], (*a)[0][2]);
    *y = M_PI / 2;
    *z = 0;
  }
  else if ((*a)[2][0] < 1)
  {
    *x = atan2 ((*a)[2][1], (*a)[2][2]);
    *y = asin (-(*a)[2][0]);
    *z = atan2 ((*a)[1][0], (*a)[0][0]);
  }
  else
  {
    *x = atan2 (-(*a)[0][1], -(*a)[0][2]);
    *y = -M_PI / 2;
    *z = 0;
  }
}
