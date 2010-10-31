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

#include "clutter-bullet-pivot.h"
#include "clutter-bullet-actor.h"

#include <btBulletDynamicsCommon.h>



#define CLUTTER_BULLET_PIVOT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_BULLET_TYPE_PIVOT, ClutterBulletPivotPrivate))



struct _ClutterBulletPivotPrivate
{
  btPoint2PointConstraint *joint;

  ClutterBulletGroup      *group;

  ClutterVertex            pivot[2];
};



enum
{
  PROP_0,
  PROP_PIVOT_0,
  PROP_PIVOT_1
};



static void clutter_bullet_pivot_get_property (GObject            *obj,
                                               guint               key,
                                               GValue             *val,
                                               GParamSpec         *spec);

static void clutter_bullet_pivot_set_property (GObject            *obj,
                                               guint               key,
                                               const GValue       *val,
                                               GParamSpec         *spec);

static void clutter_bullet_pivot_bind         (ClutterBulletJoint *joint,
                                               ClutterBulletGroup *group,
                                               GSList             *actors);

static void clutter_bullet_pivot_finalize     (GObject            *obj);



G_DEFINE_TYPE (
  ClutterBulletPivot,
  clutter_bullet_pivot,
  CLUTTER_BULLET_TYPE_JOINT
)



static void
clutter_bullet_pivot_init (ClutterBulletPivot *self)
{
  g_return_if_fail (CLUTTER_BULLET_IS_PIVOT (self));

  self->priv = CLUTTER_BULLET_PIVOT_GET_PRIVATE (self);

  self->priv->joint = NULL;
  self->priv->group = NULL;

  self->priv->pivot[0].x = 0;
  self->priv->pivot[0].y = 0;
  self->priv->pivot[0].z = 0;
  self->priv->pivot[1].x = 0;
  self->priv->pivot[1].y = 0;
  self->priv->pivot[1].z = 0;
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
                             (GParamFlags) (G_PARAM_READWRITE |
                                            G_PARAM_CONSTRUCT));

  g_object_class_install_property (glass, PROP_PIVOT_0, spec);

  spec = g_param_spec_boxed ("pivot-1",
                             "Pivot of second actor",
                             "Local pivot of second actor in pixels",
                             CLUTTER_TYPE_VERTEX,
                             (GParamFlags) (G_PARAM_READWRITE |
                                            G_PARAM_CONSTRUCT));

  g_object_class_install_property (glass, PROP_PIVOT_1, spec);
}



ClutterBulletJoint *
clutter_bullet_pivot_new (ClutterActor  *actor_0,
                          ClutterVertex *pivot_0,
                          ClutterActor  *actor_1,
                          ClutterVertex *pivot_1)
{
  gpointer ptr = g_object_new (CLUTTER_BULLET_TYPE_PIVOT,
                               "pivot-0", pivot_0,
                               "pivot-1", pivot_1,
                               NULL);

  ClutterBulletJoint *self = CLUTTER_BULLET_JOINT (ptr);

  clutter_bullet_joint_set (self, actor_0, actor_1, NULL);

  return self;
}



static void
clutter_bullet_pivot_get_property (GObject    *obj,
                                   guint       key,
                                   GValue     *val,
                                   GParamSpec *spec)
{
  ClutterBulletPivot *self;

  g_return_if_fail (CLUTTER_BULLET_IS_PIVOT (obj));

  self = CLUTTER_BULLET_PIVOT (obj);

  switch (key)
  {
    case PROP_PIVOT_0:
      if (self->priv->group != NULL && self->priv->joint != NULL)
      {
        gdouble   scale = clutter_bullet_group_get_scale (self->priv->group);
        btVector3 pivot = scale * self->priv->joint->getPivotInA ();

        self->priv->pivot[0].x = pivot.x ();
        self->priv->pivot[0].y = pivot.y ();
        self->priv->pivot[0].z = pivot.z ();
      }

      g_value_set_boxed (val, self->priv->pivot + 0);

      break;

    case PROP_PIVOT_1:
      if (self->priv->group != NULL && self->priv->joint != NULL)
      {
        gdouble   scale = clutter_bullet_group_get_scale (self->priv->group);
        btVector3 pivot = scale * self->priv->joint->getPivotInB ();

        self->priv->pivot[1].x = pivot.x ();
        self->priv->pivot[1].y = pivot.y ();
        self->priv->pivot[1].z = pivot.z ();
      }

      g_value_set_boxed (val, self->priv->pivot + 1);

      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, key, spec);
      break;
  }
}



static void
clutter_bullet_pivot_set_property (GObject      *obj,
                                   guint         key,
                                   const GValue *val,
                                   GParamSpec   *spec)
{
  ClutterBulletPivot  *self;
  const ClutterVertex *vertex;

  g_return_if_fail (CLUTTER_BULLET_IS_PIVOT (obj));

  self = CLUTTER_BULLET_PIVOT (obj);

  switch (key)
  {
    case PROP_PIVOT_0:
      vertex = (const ClutterVertex *) g_value_get_boxed (val);

      self->priv->pivot[0].x = vertex->x;
      self->priv->pivot[0].y = vertex->y;
      self->priv->pivot[0].z = vertex->z;

      if (self->priv->group != NULL && self->priv->joint != NULL)
      {
        gdouble   scale = clutter_bullet_group_get_scale (self->priv->group);
        btVector3 pivot (vertex->x, vertex->y, vertex->z);

        self->priv->joint->setPivotA (pivot / scale);
      }

      g_object_notify (obj, "pivot-0");

      break;

    case PROP_PIVOT_1:
      vertex = (const ClutterVertex *) g_value_get_boxed (val);

      self->priv->pivot[1].x = vertex->x;
      self->priv->pivot[1].y = vertex->y;
      self->priv->pivot[1].z = vertex->z;

      if (self->priv->group != NULL && self->priv->joint != NULL)
      {
        gdouble   scale = clutter_bullet_group_get_scale (self->priv->group);
        btVector3 pivot (vertex->x, vertex->y, vertex->z);

        self->priv->joint->setPivotB (pivot / scale);
      }

      g_object_notify (obj, "pivot-1");

      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, key, spec);
      break;
  }
}



static void
clutter_bullet_pivot_bind (ClutterBulletJoint *joint,
                           ClutterBulletGroup *group,
                           GSList             *actors)
{
  ClutterBulletPivot *self;
  ClutterActor       *actor[2];
  btRigidBody        *body[2];
  btVector3           pivot[2];
  gdouble             scale;

  g_return_if_fail (CLUTTER_BULLET_IS_PIVOT (joint));
  g_return_if_fail (CLUTTER_BULLET_IS_GROUP (group));

  g_return_if_fail (actors != NULL);
  g_return_if_fail (actors->next != NULL);
  g_return_if_fail (actors->next->next == NULL);
  g_return_if_fail (CLUTTER_IS_ACTOR (actors->data));
  g_return_if_fail (CLUTTER_IS_ACTOR (actors->next->data));

  self     = CLUTTER_BULLET_PIVOT (joint);
  actor[0] = CLUTTER_ACTOR (actors->data);
  actor[1] = CLUTTER_ACTOR (actors->next->data);

  if (CLUTTER_BULLET_IS_ACTOR (actor[0]))
  {
    ClutterBulletActor *shell = CLUTTER_BULLET_ACTOR (actor[0]);

    body[0] = clutter_bullet_actor_get_body (shell);
  }
  else
  {
    GObject     *obj = G_OBJECT (actor[0]);
    const gchar *key = CLUTTER_BULLET_ACTOR_BODY_KEY;

    body[0] = (btRigidBody *) g_object_get_data (obj, key);
  }

  if (CLUTTER_BULLET_IS_ACTOR (actor[1]))
  {
    ClutterBulletActor *shell = CLUTTER_BULLET_ACTOR (actor[1]);

    body[1] = clutter_bullet_actor_get_body (shell);
  }
  else
  {
    GObject     *obj = G_OBJECT (actor[1]);
    const gchar *key = CLUTTER_BULLET_ACTOR_BODY_KEY;

    body[1] = (btRigidBody *) g_object_get_data (obj, key);
  }

  g_return_if_fail (body[0] != NULL);
  g_return_if_fail (body[1] != NULL);

  scale = clutter_bullet_group_get_scale (group);

  pivot[0].setX (self->priv->pivot[0].x / scale);
  pivot[0].setY (self->priv->pivot[0].y / scale);
  pivot[0].setZ (self->priv->pivot[0].z / scale);

  pivot[1].setX (self->priv->pivot[1].x / scale);
  pivot[1].setY (self->priv->pivot[1].y / scale);
  pivot[1].setZ (self->priv->pivot[1].z / scale);

  self->priv->group = group;
  self->priv->joint = new btPoint2PointConstraint (*body[0],
                                                   *body[1],
                                                   pivot[0],
                                                   pivot[1]);

  clutter_bullet_group_get_world (group)->addConstraint (self->priv->joint);
}



static void
clutter_bullet_pivot_finalize (GObject *obj)
{
  ClutterBulletPivot *self;

  g_return_if_fail (CLUTTER_BULLET_IS_PIVOT (obj));

  self = CLUTTER_BULLET_PIVOT (obj);

  if (self->priv->group != NULL && self->priv->joint != NULL)
  {
    btDynamicsWorld *world;
    btVector3        pivot;
    gdouble          scale;

    scale = clutter_bullet_group_get_scale (self->priv->group);
    pivot = scale * self->priv->joint->getPivotInA ();

    self->priv->pivot[0].x = pivot.x ();
    self->priv->pivot[0].y = pivot.y ();
    self->priv->pivot[0].z = pivot.z ();

    pivot = scale * self->priv->joint->getPivotInB ();

    self->priv->pivot[1].x = pivot.x ();
    self->priv->pivot[1].y = pivot.y ();
    self->priv->pivot[1].z = pivot.z ();

    world = clutter_bullet_group_get_world (self->priv->group);
    world->removeConstraint (self->priv->joint);
    delete self->priv->joint;

    self->priv->group = NULL;
    self->priv->joint = NULL;
  }
}
