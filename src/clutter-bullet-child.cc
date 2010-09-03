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

#include "clutter-bullet-child.h"



#define CLUTTER_BULLET_CHILD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_BULLET_TYPE_CHILD, ClutterBulletChildPrivate))



struct _ClutterBulletChildPrivate
{
};



G_DEFINE_TYPE (ClutterBulletChild, clutter_bullet_child, CLUTTER_TYPE_CHILD_META);



static void
clutter_bullet_child_init (ClutterBulletChild *self)
{
  self->priv = CLUTTER_BULLET_CHILD_GET_PRIVATE (self);
}



static void
clutter_bullet_child_class_init (ClutterBulletChildClass *klass)
{
  g_type_class_add_private (klass, sizeof (ClutterBulletChildPrivate));
}
