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
  gboolean  fixed;

  GSList   *actors;
  GSList   *signals;

  gint      pending;
};



static void clutter_bullet_joint_setv   (ClutterBulletJoint *self,
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
  g_return_if_fail (CLUTTER_BULLET_IS_JOINT (self));

  self->priv          = CLUTTER_BULLET_JOINT_GET_PRIVATE (self);
  self->priv->fixed   = FALSE;
  self->priv->actors  = NULL;
  self->priv->signals = NULL;
  self->priv->pending = 0;
}



static void
clutter_bullet_joint_class_init (ClutterBulletJointClass *klass)
{
  g_return_if_fail (CLUTTER_BULLET_IS_JOINT_CLASS (klass));

  g_type_class_add_private (klass, sizeof (ClutterBulletJointPrivate));

  klass->bind = NULL;
}



void
clutter_bullet_joint_set (ClutterBulletJoint *self,
                          ...)
{
  va_list list;

  g_return_if_fail (CLUTTER_BULLET_IS_JOINT (self));

  va_start (list, self);

  clutter_bullet_joint_setv (self, list);

  va_end (list);
}



static void
clutter_bullet_joint_setv (ClutterBulletJoint *self,
                           va_list             list)
{
  ClutterActor *actor;

  g_return_if_fail (CLUTTER_BULLET_IS_JOINT (self));

  while ((actor = va_arg (list, ClutterActor *)) != NULL)
    clutter_bullet_joint_add (self, CLUTTER_ACTOR (actor));

  clutter_bullet_joint_fix (self);
}



void
clutter_bullet_joint_add (ClutterBulletJoint *self,
                          ClutterActor       *actor)
{
  g_return_if_fail (CLUTTER_BULLET_IS_JOINT (self));
  g_return_if_fail (CLUTTER_IS_ACTOR (actor));

  g_warn_if_fail (!self->priv->fixed);

  if (!self->priv->fixed)
    self->priv->actors = g_slist_prepend (self->priv->actors, actor);
}



void
clutter_bullet_joint_fix (ClutterBulletJoint *self)
{
  GSList *node;

  g_return_if_fail (CLUTTER_BULLET_IS_JOINT (self));

  g_warn_if_fail (!self->priv->fixed);

  if (self->priv->fixed)
    return;

  self->priv->fixed  = TRUE;
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
  ClutterBulletJoint *self;

  g_return_if_fail (CLUTTER_IS_ACTOR (obj));
  g_return_if_fail (CLUTTER_BULLET_IS_JOINT (data));

  self = CLUTTER_BULLET_JOINT (data);

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

  g_return_if_fail (CLUTTER_BULLET_IS_JOINT (self));

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
        ClutterActor *parent;

        parent = clutter_actor_get_parent (child);

        g_warning ("ClutterBulletActor has allocation but no body");

        if (CLUTTER_BULLET_IS_GROUP (parent))
        {
          ClutterBulletGroup *group;

          group = CLUTTER_BULLET_GROUP (parent);

          clutter_bullet_group_bind (group, actor);

          body = clutter_bullet_actor_get_body (shell);
        }
        else
          g_warning ("ClutterBulletActor not child of ClutterBulletGroup");
      }
    }
    else
    {
      const gchar *key;

      key  = CLUTTER_BULLET_ACTOR_BODY_KEY;
      body = g_object_get_data (G_OBJECT (actor), key);

      if (body == NULL && clutter_actor_has_allocation (actor))
      {
        ClutterActor *parent;

        parent = clutter_actor_get_parent (actor);

        if (CLUTTER_BULLET_IS_GROUP (parent))
        {
          ClutterBulletGroup *group;

          group = CLUTTER_BULLET_GROUP (parent);

          clutter_bullet_group_bind (group, actor);

          body = g_object_get_data (G_OBJECT (actor), key);
        }
        else
          g_warning ("ClutterActor not child of ClutterBulletGroup");
      }
    }

    if (body == NULL)
      self->priv->pending++;
  }
}



static void
clutter_bullet_joint_bind (ClutterBulletJoint *self)
{
  ClutterBulletGroup *group;
  GSList             *actors;
  GSList             *signals;

  g_return_if_fail (CLUTTER_BULLET_IS_JOINT (self));

  /* stop if any actors aren't done */
  if (self->priv->pending)
    return;

  group   = NULL;
  actors  = self->priv->actors;
  signals = self->priv->signals;

  self->priv->signals = NULL;

  while (actors != NULL && signals != NULL)
  {
    ClutterActor *actor  = CLUTTER_ACTOR (actors->data);
    ClutterActor *child  = actor;
    ClutterActor *parent;
    gulong       *signal = (gulong *) signals->data;

    /* obtain the correct child actor */
    if (CLUTTER_BULLET_IS_ACTOR (actor))
    {
      ClutterBulletActor *shell = CLUTTER_BULLET_ACTOR (actor);

      child = clutter_bullet_actor_get_actor (shell);

      if (child == NULL)
        child = actor;
    }

    parent = clutter_actor_get_parent (child);

    /* all actors need the same ClutterBulletGroup as parent */
    g_warn_if_fail (CLUTTER_BULLET_IS_GROUP (parent));
    g_warn_if_fail (group == NULL || group == CLUTTER_BULLET_GROUP (parent));

    group = CLUTTER_BULLET_GROUP (parent);

    g_signal_handler_disconnect (actor, *signal);
    g_free (signal);

    actors  = actors->next;
    signals = g_slist_delete_link (signals, signals);
  }

  g_warn_if_fail (actors  == NULL);
  g_warn_if_fail (signals == NULL);

  actors = self->priv->actors;

  CLUTTER_BULLET_JOINT_GET_CLASS (self)->bind (self, group, actors);
}
