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

#include "clutter-bullet-joint.h"
#include "clutter-bullet-group.h"



#define CLUTTER_BULLET_JOINT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_BULLET_TYPE_JOINT, ClutterBulletJointPrivate))



struct _ClutterBulletJointPrivate
{
  ClutterBulletGroup *group;

  GSList             *actor;

  gboolean            ready;
};



enum
{
  PROP_0,
  PROP_GROUP,
  PROP_ACTOR
};



static void clutter_bullet_joint_get_property (GObject            *obj,
                                               guint               key,
                                               GValue             *val,
                                               GParamSpec         *spec);

static void clutter_bullet_joint_set_property (GObject            *obj,
                                               guint               key,
                                               const GValue       *val,
                                               GParamSpec         *spec);

static void clutter_bullet_joint_bind         (ClutterBulletJoint *self);



G_DEFINE_ABSTRACT_TYPE (
  ClutterBulletJoint,
  clutter_bullet_joint,
  G_TYPE_OBJECT
);



static void
clutter_bullet_joint_init (ClutterBulletJoint *self)
{
  self->priv        = CLUTTER_BULLET_JOINT_GET_PRIVATE (self);
  self->priv->group = NULL;
  self->priv->actor = NULL;
  self->priv->ready = FALSE;
}



static void
clutter_bullet_joint_class_init (ClutterBulletJointClass *klass)
{
  GObjectClass *glass = G_OBJECT_CLASS (klass);
  GParamSpec   *spec;

  g_type_class_add_private (klass, sizeof (ClutterBulletJointPrivate));

  glass->get_property = clutter_bullet_joint_get_property;
  glass->set_property = clutter_bullet_joint_set_property;

  spec = g_param_spec_object ("group",
                              "Actor container",
                              "Parent container of actor parameters",
                              CLUTTER_BULLET_TYPE_GROUP,
                              (GParamFlags) (G_PARAM_READWRITE |
                                             G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (glass, PROP_GROUP, spec);

  spec = g_param_spec_pointer ("actor",
                               "Actor list",
                               "List of actor parameters",
                               G_PARAM_READABLE);

  g_object_class_install_property (glass, PROP_ACTOR, spec);
}



static void
clutter_bullet_joint_get_property (GObject    *obj,
                                   guint       key,
                                   GValue     *val,
                                   GParamSpec *spec)
{
  ClutterBulletJoint *self = CLUTTER_BULLET_JOINT (obj);

  switch (key)
  {
    case PROP_GROUP:
      g_value_set_object (val, self->priv->group);
      break;

    case PROP_ACTOR:
      g_value_set_pointer (val, self->priv->actor);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, key, spec);
      break;
  }
}



static void
clutter_bullet_joint_set_property (GObject      *obj,
                                   guint         key,
                                   const GValue *val,
                                   GParamSpec   *spec)
{
  ClutterBulletJoint *self = CLUTTER_BULLET_JOINT (obj);

  switch (key)
  {
    case PROP_GROUP:
      self->priv->group = CLUTTER_BULLET_GROUP (g_value_get_object (val));
      break;

    case PROP_ACTOR:
      self->priv->actor = (GSList *) g_value_get_pointer (val);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, key, spec);
      break;
  }
}



void
clutter_bullet_joint_add_actor (ClutterBulletJoint *self,
                                ClutterActor       *actor)
{
  self->priv->actor = g_slist_prepend (self->priv->actor, actor);
}



void
clutter_bullet_joint_listen (ClutterBulletJoint *self)
{
  if (!self->priv->ready)
  {
    self->priv->ready = TRUE;
    self->priv->actor = g_slist_reverse (self->priv->actor);

    /* XXX */
  }
}



static void
clutter_bullet_joint_bind (ClutterBulletJoint *self)
{
  CLUTTER_BULLET_JOINT_GET_CLASS (self)->bind (self);
}
