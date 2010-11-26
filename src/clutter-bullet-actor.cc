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
 * SECTION:clutter-bullet-actor
 * @short_description: Interface for physics-enabled actors
 * @title: ClutterBulletActor
 * @see_also: #ClutterBulletGroup
 *
 * #ClutterBulletActor is the interface implemented by a #ClutterActor
 * for inserting itself into a physics simulation governed by a
 * #ClutterBulletGroup.
 *
 * In general, when a non-#ClutterBulletActor #ClutterActor is added
 * to a #ClutterBulletGroup, it's treated as a flat immobile rectangle
 * in space that other #ClutterBulletActor<!-- -->s can collide with.
 * Implementing #ClutterBulletActor allows a #ClutterActor to
 * customize this treatment.
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



/**
 * clutter_bullet_actor_get_actor:
 * @self: a #ClutterBulletActor
 *
 * Gets the target #ClutterActor added when @self is inserted into a
 * #ClutterBulletGroup.  It could even be @self itself.
 *
 * Returns: @self's target #ClutterActor
 */
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



/**
 * clutter_bullet_actor_get_body:
 * @self: a #ClutterBulletActor
 *
 * Gets the physical representation of @self, if it exists.
 *
 * Returns: @self's physical representation or #NULL if none
 */
btRigidBody *
clutter_bullet_actor_get_body (ClutterBulletActor *self)
{
  btRigidBody *body;

  g_object_get (self, "body", &body, NULL);

  return body;
}



/**
 * clutter_bullet_actor_bind:
 * @self:  a #ClutterBulletActor
 * @group: @self's new parent #ClutterBulletGroup
 *
 * Creates @self's body and inserts it into @group's physics
 * simulation.  Only called when @self's real actor has a valid
 * allocation.
 */
void
clutter_bullet_actor_bind (ClutterBulletActor *self,
                           ClutterBulletGroup *group)
{
  btRigidBody *body = clutter_bullet_actor_get_body (self);

  CLUTTER_BULLET_ACTOR_GET_INTERFACE (self)->bind (self, group);

  if (clutter_bullet_actor_get_body (self) != body)
    g_object_notify (G_OBJECT (self), "body");
}



/**
 * clutter_bullet_actor_unbind:
 * @self:  a #ClutterBulletActor
 * @group: @self's parent #ClutterBulletGroup
 *
 * Removes @self's body from @group's physics simulation and destroys
 * it.
 */
void
clutter_bullet_actor_unbind (ClutterBulletActor *self,
                             ClutterBulletGroup *group)
{
  btRigidBody *body = clutter_bullet_actor_get_body (self);

  CLUTTER_BULLET_ACTOR_GET_INTERFACE (self)->unbind (self, group);

  if (clutter_bullet_actor_get_body (self) != body)
    g_object_notify (G_OBJECT (self), "body");
}
