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



#ifndef __CLUTTER_BULLET_ACTOR_H__
#define __CLUTTER_BULLET_ACTOR_H__



G_BEGIN_DECLS



#define CLUTTER_BULLET_TYPE_ACTOR                (clutter_bullet_actor_get_type ())
#define CLUTTER_BULLET_ACTOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_BULLET_TYPE_ACTOR, ClutterBulletActor))
#define CLUTTER_BULLET_IS_ACTOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_BULLET_TYPE_ACTOR))
#define CLUTTER_BULLET_ACTOR_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), CLUTTER_BULLET_TYPE_ACTOR, ClutterBulletActorInterface))



typedef struct _ClutterBulletActor          ClutterBulletActor;
typedef struct _ClutterBulletActorInterface ClutterBulletActorInterface;



struct _ClutterBulletActorInterface
{
  GTypeInterface parent;
};



GType clutter_bullet_actor_get_type (void);



G_END_DECLS



#endif /* __CLUTTER_BULLET_ACTOR_H__ */
