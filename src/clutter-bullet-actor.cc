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

#include "clutter-bullet-actor.h"

#include <BulletCollision/CollisionShapes/btBox2dShape.h>
#include <btBulletDynamicsCommon.h>

#include "clutter-bullet-private.h"



static GHashTable *actor_body;



G_DEFINE_INTERFACE (
  ClutterBulletActor,
  clutter_bullet_actor,
  G_TYPE_INVALID
);



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
clutter_bullet_actor_get_actor (ClutterActor *self)
{
  ClutterActor *actor = self;

  if (CLUTTER_BULLET_IS_ACTOR (self))
  {
    g_object_get (self, "actor", &actor, NULL);

    if (actor != NULL)
      g_object_unref (actor);
    else
      actor = self;
  }

  return actor;
}



void
clutter_bullet_actor_bind (ClutterActor       *self,
                           ClutterBulletGroup *group)
{
  if (CLUTTER_BULLET_IS_ACTOR (self))
  {
    ClutterBulletActor *actor = CLUTTER_BULLET_ACTOR (self);

    CLUTTER_BULLET_ACTOR_GET_INTERFACE (actor)->bind (actor, group);
  }
  else
  {
    if (actor_body == NULL)
      actor_body = g_hash_table_new (NULL, NULL);

    if (g_hash_table_lookup (actor_body, self) == NULL)
    {
      btCollisionShape *shape;
      btVector3         tensor;
      gdouble           scale;
      gfloat            w, h;

      scale = clutter_bullet_group_get_scale (group);

      clutter_actor_get_size (self, &w, &h);

      w /= scale;
      h /= scale;

      shape = new btBox2dShape (btVector3 (w / 2, h / 2, 0));

      shape->setMargin (0);
      shape->calculateLocalInertia (0, tensor);

      btRigidBody *b = new btRigidBody (
        btRigidBody::btRigidBodyConstructionInfo (
          0,
          new ClutterBulletMotionState (self, scale),
          shape,
          tensor
        )
      );

      clutter_bullet_group_get_world (group)->addRigidBody (b);

      g_hash_table_replace (actor_body, self, b);
    }
  }
}



void
clutter_bullet_actor_unbind (ClutterActor       *self,
                             ClutterBulletGroup *group)
{
  if (CLUTTER_BULLET_IS_ACTOR (self))
  {
    ClutterBulletActor *actor = CLUTTER_BULLET_ACTOR (self);

    CLUTTER_BULLET_ACTOR_GET_INTERFACE (actor)->unbind (actor, group);
  }
  else
  {
    btRigidBody *b;

    if (actor_body == NULL)
      actor_body = g_hash_table_new (NULL, NULL);

    if ((b = (btRigidBody *) g_hash_table_lookup (actor_body, self)) != NULL)
    {
      g_hash_table_remove (actor_body, self);

      clutter_bullet_group_get_world (group)->removeRigidBody (b);

      delete b->getCollisionShape ();
      delete b->getMotionState ();
      delete b;
    }
  }
}
