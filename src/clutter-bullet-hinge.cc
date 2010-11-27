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



/**
 * SECTION:clutter-bullet-hinge
 * @short_description: Hinge constraint between two actors
 * @title: ClutterBulletHinge
 *
 * #ClutterBulletHinge fixes two actors to rotate along a common local
 * axis.  Each actor defines its local axis using two pivot points in
 * its own local coordinates, and the #ClutterBulletHinge constrains
 * both axes to match each other in global space.
 */
 


#include <clutter/clutter.h>

#include "clutter-bullet-hinge.h"
#include "clutter-bullet-actor.h"

#include <btBulletDynamicsCommon.h>



#define CLUTTER_BULLET_HINGE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_BULLET_TYPE_HINGE, ClutterBulletHingePrivate))



struct _ClutterBulletHingePrivate
{
  btHingeConstraint  *joint;

  ClutterBulletGroup *group;

  ClutterVertex       pivot[2][2];
};



enum
{
  PROP_0,
  PROP_PIVOT_0_0,
  PROP_PIVOT_0_1,
  PROP_PIVOT_1_0,
  PROP_PIVOT_1_1
};



static void clutter_bullet_hinge_get_property (GObject            *obj,
                                               guint               key,
                                               GValue             *val,
                                               GParamSpec         *spec);

static void clutter_bullet_hinge_set_property (GObject            *obj,
                                               guint               key,
                                               const GValue       *val,
                                               GParamSpec         *spec);

static void clutter_bullet_hinge_bind         (ClutterBulletJoint *joint,
                                               ClutterBulletGroup *group,
                                               GSList             *actors);

static void clutter_bullet_hinge_finalize     (GObject            *obj);



G_DEFINE_TYPE (
  ClutterBulletHinge,
  clutter_bullet_hinge,
  CLUTTER_BULLET_TYPE_JOINT
)



static void
clutter_bullet_hinge_init (ClutterBulletHinge *self)
{
  g_return_if_fail (CLUTTER_BULLET_IS_HINGE (self));

  self->priv = CLUTTER_BULLET_HINGE_GET_PRIVATE (self);

  self->priv->joint = NULL;
  self->priv->group = NULL;

  self->priv->pivot[0][0].x = 0;
  self->priv->pivot[0][0].y = 0;
  self->priv->pivot[0][0].z = 0;
  self->priv->pivot[0][1].x = 0;
  self->priv->pivot[0][1].y = 0;
  self->priv->pivot[0][1].z = 0;
  self->priv->pivot[1][0].x = 0;
  self->priv->pivot[1][0].y = 0;
  self->priv->pivot[1][0].z = 0;
  self->priv->pivot[1][1].x = 0;
  self->priv->pivot[1][1].y = 0;
  self->priv->pivot[1][1].z = 0;
}



static void
clutter_bullet_hinge_class_init (ClutterBulletHingeClass *klass)
{
  GObjectClass            *glass;
  ClutterBulletJointClass *jlass;
  GParamSpec              *spec;

  g_return_if_fail (CLUTTER_BULLET_IS_HINGE_CLASS (klass));

  g_type_class_add_private (klass, sizeof (ClutterBulletHingePrivate));

  glass = G_OBJECT_CLASS (klass);
  jlass = CLUTTER_BULLET_JOINT_CLASS (klass);

  glass->get_property = clutter_bullet_hinge_get_property;
  glass->set_property = clutter_bullet_hinge_set_property;
  glass->finalize     = clutter_bullet_hinge_finalize;
  jlass->bind         = clutter_bullet_hinge_bind;

  spec = g_param_spec_boxed ("pivot-0-0",
                             "First pivot of first actor",
                             "First pivot of first actor in pixels",
                             CLUTTER_TYPE_VERTEX,
                             (GParamFlags) (G_PARAM_READWRITE |
                                            G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (glass, PROP_PIVOT_0_0, spec);

  spec = g_param_spec_boxed ("pivot-0-1",
                             "Second pivot of first actor",
                             "Second pivot of first actor in pixels",
                             CLUTTER_TYPE_VERTEX,
                             (GParamFlags) (G_PARAM_READWRITE |
                                            G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (glass, PROP_PIVOT_0_1, spec);

  spec = g_param_spec_boxed ("pivot-1-0",
                             "First pivot of second actor",
                             "First pivot of second actor in pixels",
                             CLUTTER_TYPE_VERTEX,
                             (GParamFlags) (G_PARAM_READWRITE |
                                            G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (glass, PROP_PIVOT_1_0, spec);

  spec = g_param_spec_boxed ("pivot-1-1",
                             "Second pivot of second actor",
                             "Second pivot of second actor in pixels",
                             CLUTTER_TYPE_VERTEX,
                             (GParamFlags) (G_PARAM_READWRITE |
                                            G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (glass, PROP_PIVOT_1_1, spec);
}



/**
 * clutter_bullet_hinge_new:
 * @actor_0:   a #ClutterActor
 * @pivot_0_0: one of @actor_0's pivots in local coordinates
 * @pivot_0_1: another of @actor_0's pivots in local coordinates
 * @actor_1:   a #ClutterActor
 * @pivot_1_0: one of @actor_1's pivots in local coordinates
 * @pivot_1_1: another of @actor_1's pivots in local coordinates
 *
 * Joins @actor_0 and @actor_1 together using a hinge joint
 * constraint.  @actor_0 and @actor_1 are joined so that the axis
 * defined by @pivot_0_0 and @pivot_0_1 is aligned with the axis
 * defined by @pivot_1_0 and @pivot_1_1 in global space.
 *
 * Returns: a #ClutterBulletHinge
 */
ClutterBulletJoint *
clutter_bullet_hinge_new (ClutterActor  *actor_0,
                          ClutterVertex *pivot_0_0,
                          ClutterVertex *pivot_0_1,
                          ClutterActor  *actor_1,
                          ClutterVertex *pivot_1_0,
                          ClutterVertex *pivot_1_1)
{
  gpointer ptr = g_object_new (CLUTTER_BULLET_TYPE_HINGE,
                               "pivot-0-0", pivot_0_0,
                               "pivot-0-1", pivot_0_1,
                               "pivot-1-0", pivot_1_0,
                               "pivot-1-1", pivot_1_1,
                               NULL);

  ClutterBulletJoint *self = CLUTTER_BULLET_JOINT (ptr);

  clutter_bullet_joint_set (self, actor_0, actor_1, NULL);

  return self;
}



static void
clutter_bullet_hinge_get_property (GObject    *obj,
                                   guint       key,
                                   GValue     *val,
                                   GParamSpec *spec)
{
  ClutterBulletHinge *self;

  g_return_if_fail (CLUTTER_BULLET_IS_HINGE (obj));

  self = CLUTTER_BULLET_HINGE (obj);

  switch (key)
  {
    case PROP_PIVOT_0_0:
      g_value_set_boxed (val, self->priv->pivot[0] + 0);
      break;

    case PROP_PIVOT_0_1:
      g_value_set_boxed (val, self->priv->pivot[0] + 1);
      break;

    case PROP_PIVOT_1_0:
      g_value_set_boxed (val, self->priv->pivot[1] + 0);
      break;

    case PROP_PIVOT_1_1:
      g_value_set_boxed (val, self->priv->pivot[1] + 1);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, key, spec);
      break;
  }
}



static void
clutter_bullet_hinge_set_property (GObject      *obj,
                                   guint         key,
                                   const GValue *val,
                                   GParamSpec   *spec)
{
  ClutterBulletHinge  *self;
  const ClutterVertex *vertex;

  g_return_if_fail (CLUTTER_BULLET_IS_HINGE (obj));

  self = CLUTTER_BULLET_HINGE (obj);

  switch (key)
  {
    case PROP_PIVOT_0_0:
      vertex = (const ClutterVertex *) g_value_get_boxed (val);

      g_return_if_fail (vertex != NULL);

      self->priv->pivot[0][0].x = vertex->x;
      self->priv->pivot[0][0].y = vertex->y;
      self->priv->pivot[0][0].z = vertex->z;

      g_object_notify (obj, "pivot-0-0");

      break;

    case PROP_PIVOT_0_1:
      vertex = (const ClutterVertex *) g_value_get_boxed (val);

      g_return_if_fail (vertex != NULL);

      self->priv->pivot[0][1].x = vertex->x;
      self->priv->pivot[0][1].y = vertex->y;
      self->priv->pivot[0][1].z = vertex->z;

      g_object_notify (obj, "pivot-0-1");

      break;

    case PROP_PIVOT_1_0:
      vertex = (const ClutterVertex *) g_value_get_boxed (val);

      g_return_if_fail (vertex != NULL);

      self->priv->pivot[1][0].x = vertex->x;
      self->priv->pivot[1][0].y = vertex->y;
      self->priv->pivot[1][0].z = vertex->z;

      g_object_notify (obj, "pivot-1-0");

      break;

    case PROP_PIVOT_1_1:
      vertex = (const ClutterVertex *) g_value_get_boxed (val);

      g_return_if_fail (vertex != NULL);

      self->priv->pivot[1][1].x = vertex->x;
      self->priv->pivot[1][1].y = vertex->y;
      self->priv->pivot[1][1].z = vertex->z;

      g_object_notify (obj, "pivot-1-1");

      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, key, spec);
      break;
  }
}



static void
clutter_bullet_hinge_bind (ClutterBulletJoint *joint,
                           ClutterBulletGroup *group,
                           GSList             *actors)
{
  ClutterBulletHinge *self;
  ClutterActor       *actor[2];
  btRigidBody        *body[2];
  btVector3           pivot[2][2];
  gdouble             scale;

  g_return_if_fail (CLUTTER_BULLET_IS_HINGE (joint));
  g_return_if_fail (CLUTTER_BULLET_IS_GROUP (group));

  g_return_if_fail (actors != NULL);
  g_return_if_fail (actors->next != NULL);
  g_return_if_fail (actors->next->next == NULL);
  g_return_if_fail (CLUTTER_IS_ACTOR (actors->data));
  g_return_if_fail (CLUTTER_IS_ACTOR (actors->next->data));

  self     = CLUTTER_BULLET_HINGE (joint);
  actor[0] = CLUTTER_ACTOR (actors->data);
  actor[1] = CLUTTER_ACTOR (actors->next->data);
  body[0]  = clutter_bullet_group_get_body (group, actor[0]);
  body[1]  = clutter_bullet_group_get_body (group, actor[1]);

  g_return_if_fail (body[0] != NULL);
  g_return_if_fail (body[1] != NULL);

  scale = clutter_bullet_group_get_scale (group);

  pivot[0][0].setX (self->priv->pivot[0][0].x / scale);
  pivot[0][0].setY (self->priv->pivot[0][0].y / scale);
  pivot[0][0].setZ (self->priv->pivot[0][0].z / scale);

  pivot[0][1].setX (self->priv->pivot[0][1].x / scale);
  pivot[0][1].setY (self->priv->pivot[0][1].y / scale);
  pivot[0][1].setZ (self->priv->pivot[0][1].z / scale);

  pivot[1][0].setX (self->priv->pivot[1][0].x / scale);
  pivot[1][0].setY (self->priv->pivot[1][0].y / scale);
  pivot[1][0].setZ (self->priv->pivot[1][0].z / scale);

  pivot[1][1].setX (self->priv->pivot[1][1].x / scale);
  pivot[1][1].setY (self->priv->pivot[1][1].y / scale);
  pivot[1][1].setZ (self->priv->pivot[1][1].z / scale);

  self->priv->group = group;
  self->priv->joint = new btHingeConstraint (*body[0],
                                             *body[1],
                                             pivot[0][0],
                                             pivot[1][0],
                                             pivot[0][1] - pivot[0][0],
                                             pivot[1][1] - pivot[1][0]);

  clutter_bullet_group_get_world (group)->addConstraint (self->priv->joint);
}



static void
clutter_bullet_hinge_finalize (GObject *obj)
{
  ClutterBulletHinge *self;

  g_return_if_fail (CLUTTER_BULLET_IS_HINGE (obj));

  self = CLUTTER_BULLET_HINGE (obj);

  if (self->priv->group != NULL && self->priv->joint != NULL)
  {
    btDynamicsWorld *world;

    world = clutter_bullet_group_get_world (self->priv->group);
    world->removeConstraint (self->priv->joint);
    delete self->priv->joint;

    self->priv->group = NULL;
    self->priv->joint = NULL;
  }
}
