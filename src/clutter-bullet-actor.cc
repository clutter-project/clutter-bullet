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

#include "clutter-bullet-actor.h"



G_DEFINE_INTERFACE (
  ClutterBulletActor,
  clutter_bullet_actor,
  CLUTTER_TYPE_ACTOR
)



static void
clutter_bullet_actor_default_init (ClutterBulletActorInterface *klass)
{
  static GParamSpec *spec = NULL;

  if (spec == NULL)
  {
    spec = g_param_spec_object ("actor",
                                "Target actor",
                                "Target actor to add to group",
                                CLUTTER_TYPE_ACTOR,
                                (GParamFlags) (G_PARAM_READWRITE |
                                               G_PARAM_CONSTRUCT_ONLY));

    g_object_interface_install_property (klass, spec);

    spec = g_param_spec_pointer ("body",
                                 "Rigid body",
                                 "Physical representation of actor",
                                 G_PARAM_READABLE);

    g_object_interface_install_property (klass, spec);
  }
}



ClutterActor *
clutter_bullet_actor_get_actor (ClutterBulletActor *self)
{
  ClutterActor *actor;

  g_object_get (self, "actor", &actor, NULL);

  if (actor == NULL)
    actor = CLUTTER_ACTOR (self);
  else
    g_object_unref (actor);

  return actor;
}



btRigidBody *
clutter_bullet_actor_get_body (ClutterBulletActor *self)
{
  btRigidBody *body;

  g_object_get (self, "body", &body, NULL);

  return body;
}



void
clutter_bullet_actor_bind (ClutterBulletActor *self,
                           ClutterBulletGroup *group)
{
  btRigidBody *body = clutter_bullet_actor_get_body (self);

  CLUTTER_BULLET_ACTOR_GET_INTERFACE (self)->bind (self, group);

  if (clutter_bullet_actor_get_body (self) != body)
    g_object_notify (G_OBJECT (self), "body");
}



void
clutter_bullet_actor_unbind (ClutterBulletActor *self,
                             ClutterBulletGroup *group)
{
  btRigidBody *body = clutter_bullet_actor_get_body (self);

  CLUTTER_BULLET_ACTOR_GET_INTERFACE (self)->unbind (self, group);

  if (clutter_bullet_actor_get_body (self) != body)
    g_object_notify (G_OBJECT (self), "body");
}
