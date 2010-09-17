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



ClutterBulletMotionState::ClutterBulletMotionState (ClutterActor *a)
: actor (a)
{
}



void
ClutterBulletMotionState::getWorldTransform (btTransform &t) const
{
  CoglMatrix    a;
  ClutterVertex x;
  btVector3     y;

  clutter_actor_get_size (actor, &x.x, &x.y);

  x.x /= 2;
  x.y /= 2;
  x.z  = 0;

  clutter_actor_get_transformation_matrix (actor, &a);

  y.setX (a.xx * x.x + a.xy * x.y + a.xz * x.z + a.xw);
  y.setY (a.yx * x.x + a.yy * x.y + a.yz * x.z + a.yw);
  y.setZ (a.zx * x.x + a.zy * x.y + a.zz * x.z + a.zw);
  y.setW (0);

  t.setOrigin (y);
}



void
ClutterBulletMotionState::setWorldTransform (const btTransform &t)
{
}
