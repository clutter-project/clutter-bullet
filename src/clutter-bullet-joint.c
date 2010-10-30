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

#include "clutter-bullet-joint.h"
#include "clutter-bullet-actor.h"



#define CLUTTER_BULLET_JOINT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_BULLET_TYPE_JOINT, ClutterBulletJointPrivate))



struct _ClutterBulletJointPrivate
{
  ClutterBulletGroup *group;

  GSList             *actors;
  GSList             *signals;

  gint                pending;
};



static void clutter_bullet_joint_setv   (ClutterBulletJoint *self,
                                         ClutterBulletGroup *group,
                                         va_list             list);

static void clutter_bullet_joint_check  (GObject            *obj,
                                         GParamSpec         *spec,
                                         gpointer            data);

static void clutter_bullet_joint_update (ClutterBulletJoint *self);

static void clutter_bullet_joint_bind   (ClutterBulletJoint *self);



G_DEFINE_ABSTRACT_TYPE (
  ClutterBulletJoint,
  clutter_bullet_joint,
  G_TYPE_OBJECT
)



static void
clutter_bullet_joint_init (ClutterBulletJoint *self)
{
  self->priv          = CLUTTER_BULLET_JOINT_GET_PRIVATE (self);
  self->priv->group   = NULL;
  self->priv->actors  = NULL;
  self->priv->signals = NULL;
  self->priv->pending = 0;
}



static void
clutter_bullet_joint_class_init (ClutterBulletJointClass *klass)
{
  g_type_class_add_private (klass, sizeof (ClutterBulletJointPrivate));

  klass->bind = NULL;
}



void
clutter_bullet_joint_set (ClutterBulletJoint *self,
                          ClutterBulletGroup *group,
                          ...)
{
  va_list list;

  va_start (list, group);

  clutter_bullet_joint_setv (self, group, list);

  va_end (list);
}



static void
clutter_bullet_joint_setv (ClutterBulletJoint *self,
                           ClutterBulletGroup *group,
                           va_list             list)
{
  ClutterActor *actor;

  while ((actor = va_arg (list, ClutterActor *)) != NULL)
    clutter_bullet_joint_add (self, actor);

  clutter_bullet_joint_fix (self, group);
}



void
clutter_bullet_joint_add (ClutterBulletJoint *self,
                          ClutterActor       *actor)
{
  if (self->priv->group == NULL)
    self->priv->actors = g_slist_prepend (self->priv->actors, actor);
}



void
clutter_bullet_joint_fix (ClutterBulletJoint *self,
                          ClutterBulletGroup *group)
{
  GSList *node;

  if (self->priv->group != NULL)
    return;

  self->priv->group  = group;
  self->priv->actors = g_slist_reverse (self->priv->actors);

  clutter_bullet_joint_update (self);

  if (self->priv->pending)
  {
    for (node = self->priv->actors; node != NULL; node = node->next)
    {
      GCallback     call   = G_CALLBACK (clutter_bullet_joint_check);
      ClutterActor *actor  = CLUTTER_ACTOR (node->data);
      gulong       *signal = g_new (gulong, 1);
      const gchar  *name;

      if (CLUTTER_BULLET_IS_ACTOR (actor))
        name = "notify::body";
      else
        name = "notify::allocation";

      *signal = g_signal_connect (actor, name, call, self);

      self->priv->signals = g_slist_prepend (self->priv->signals, signal);
    }

    self->priv->signals = g_slist_reverse (self->priv->signals);
  }
  else
    clutter_bullet_joint_bind (self);
}



static void
clutter_bullet_joint_check (GObject    *obj,
                            GParamSpec *spec,
                            gpointer    data)
{
  ClutterBulletJoint *self = CLUTTER_BULLET_JOINT (data);

  if (!--self->priv->pending)
  {
    clutter_bullet_joint_update (self);

    if (!self->priv->pending)
      clutter_bullet_joint_bind (self);
  }
}



static void
clutter_bullet_joint_update (ClutterBulletJoint *self)
{
  GSList *node;

  self->priv->pending = 0;

  for (node = self->priv->actors; node != NULL; node = node->next)
  {
    ClutterActor *actor;
    gpointer      body;

    actor = CLUTTER_ACTOR (node->data);

    if (CLUTTER_BULLET_IS_ACTOR (actor))
    {
      ClutterBulletActor *shell;
      ClutterActor       *child;

      shell = CLUTTER_BULLET_ACTOR (actor);
      child = clutter_bullet_actor_get_actor (shell);
      body  = clutter_bullet_actor_get_body  (shell);

      if (child == NULL)
        child = actor;

      if (body == NULL && clutter_actor_has_allocation (child))
      {
        clutter_bullet_group_bind (self->priv->group, actor);

        body = clutter_bullet_actor_get_body (shell);
      }
    }
    else
    {
      const gchar *key;

      key  = CLUTTER_BULLET_ACTOR_BODY_KEY;
      body = g_object_get_data (G_OBJECT (actor), key);

      if (body == NULL && clutter_actor_has_allocation (actor))
      {
        clutter_bullet_group_bind (self->priv->group, actor);

        body = g_object_get_data (G_OBJECT (actor), key);
      }
    }

    if (body == NULL)
      self->priv->pending++;
  }
}



static void
clutter_bullet_joint_bind (ClutterBulletJoint *self)
{
  ClutterBulletGroup *group   = self->priv->group;
  GSList             *actors  = self->priv->actors;
  GSList             *signals = self->priv->signals;

  if (self->priv->pending)
    return;

  self->priv->signals = NULL;

  while (actors != NULL && signals != NULL)
  {
    ClutterActor *actor  = CLUTTER_ACTOR (actors->data);
    gulong       *signal = (gulong *) signals->data;

    g_signal_handler_disconnect (actor, *signal);
    g_free (signal);

    actors  = actors->next;
    signals = g_slist_delete_link (signals, signals);
  }

  actors = self->priv->actors;

  CLUTTER_BULLET_JOINT_GET_CLASS (self)->bind (self, group, actors);
}
