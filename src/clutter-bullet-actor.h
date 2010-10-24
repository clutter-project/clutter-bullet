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



#ifndef __CLUTTER_BULLET_ACTOR_H__
#define __CLUTTER_BULLET_ACTOR_H__



#include "clutter-bullet-group.h"



G_BEGIN_DECLS



#define CLUTTER_BULLET_TYPE_ACTOR                (clutter_bullet_actor_get_type ())
#define CLUTTER_BULLET_ACTOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_BULLET_TYPE_ACTOR, ClutterBulletActor))
#define CLUTTER_BULLET_IS_ACTOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_BULLET_TYPE_ACTOR))
#define CLUTTER_BULLET_ACTOR_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), CLUTTER_BULLET_TYPE_ACTOR, ClutterBulletActorInterface))



typedef struct _ClutterBulletActor          ClutterBulletActor;
typedef struct _ClutterBulletActorInterface ClutterBulletActorInterface;

typedef struct btRigidBody                  btRigidBody;



struct _ClutterBulletActorInterface
{
  GTypeInterface   parent;

  void           (*bind)   (ClutterBulletActor *,
                            ClutterBulletGroup *);

  void           (*unbind) (ClutterBulletActor *,
                            ClutterBulletGroup *);
};



GType          clutter_bullet_actor_get_type  (void);

ClutterActor * clutter_bullet_actor_get_actor (ClutterBulletActor *self);

btRigidBody *  clutter_bullet_actor_get_body  (ClutterBulletActor *self);

void           clutter_bullet_actor_bind      (ClutterBulletActor *self,
                                               ClutterBulletGroup *group);

void           clutter_bullet_actor_unbind    (ClutterBulletActor *self,
                                               ClutterBulletGroup *group);



G_END_DECLS



#endif /* __CLUTTER_BULLET_ACTOR_H__ */
