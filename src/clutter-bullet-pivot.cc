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



#include "clutter-bullet-pivot.h"



#define CLUTTER_BULLET_PIVOT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_BULLET_TYPE_PIVOT, ClutterBulletPivotPrivate))



struct _ClutterBulletPivotPrivate
{
  ClutterBulletGroup      *group;

  btPoint2PointConstraint *joint;
};



enum
{
  PROP_0,
  PROP_PIVOT_0,
  PROP_PIVOT_1
};



G_DEFINE_TYPE (
  ClutterBulletPivot,
  clutter_bullet_pivot,
  CLUTTER_BULLET_TYPE_JOINT
)



static void
clutter_bullet_pivot_init (ClutterBulletPivot *self)
{
  g_return_if_fail (CLUTTER_BULLET_IS_PIVOT (self));

  self->priv        = CLUTTER_BULLET_PIVOT_GET_PRIVATE (self);
  self->priv->group = NULL;
  self->priv->joint = NULL;
}



static void
clutter_bullet_pivot_class_init (ClutterBulletPivotClass *klass)
{
  GObjectClass            *glass;
  ClutterBulletJointClass *jlass;
  GParamSpec              *spec;

  g_return_if_fail (CLUTTER_BULLET_IS_PIVOT_CLASS (klass));

  g_type_class_add_private (klass, sizeof (ClutterBulletPivotPrivate));

  glass = G_OBJECT_CLASS (klass);
  jlass = CLUTTER_BULLET_JOINT_CLASS (klass);

  glass->get_property = clutter_bullet_pivot_get_property;
  glass->set_property = clutter_bullet_pivot_set_property;
  glass->finalize     = clutter_bullet_pivot_finalize;
  jlass->bind         = clutter_bullet_pivot_bind;

  spec = g_param_spec_boxed ("pivot-0",
                             "Pivot of first actor",
                             "Local pivot of first actor in pixels",
                             CLUTTER_TYPE_VERTEX,
                             G_PARAM_READWRITE);

  g_object_class_install_property (glass, PROP_PIVOT_0, spec);

  spec = g_param_spec_boxed ("pivot-1",
                             "Pivot of second actor",
                             "Local pivot of second actor in pixels",
                             CLUTTER_TYPE_VERTEX,
                             G_PARAM_READWRITE);

  g_object_class_install_property (glass, PROP_PIVOT_1, spec);
}



ClutterBulletJoint *
clutter_bullet_pivot_new (ClutterActor  *actor_0,
                          ClutterVertex *pivot_0,
                          ClutterActor  *actor_1,
                          ClutterVertex *pivot_1)
{
  /* XXX */
}



static void
clutter_bullet_pivot_get_property (GObject    *obj,
                                   guint       key,
                                   GValue     *val,
                                   GParamSpec *spec)
{
  /* XXX */
}



static void
clutter_bullet_pivot_set_property (GObject      *obj,
                                   guint         key,
                                   const GValue *val,
                                   GParamSpec   *spec)
{
  /* XXX */
}



static void
clutter_bullet_pivot_bind (ClutterBulletJoint *joint,
                           ClutterBulletGroup *group,
                           GSList             *actors)
{
  ClutterBulletPivot *self;

  g_return_if_fail (CLUTTER_BULLET_IS_PIVOT (joint));

  self = CLUTTER_BULLET_PIVOT (joint);

  /* XXX */
}



static void
clutter_bullet_pivot_finalize (GObject *obj)
{
  ClutterBulletPivot *self;

  g_return_if_fail (CLUTTER_BULLET_IS_PIVOT (obj));

  self = CLUTTER_BULLET_PIVOT (obj);

  /* XXX */
}
