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



#ifndef __CLUTTER_BULLET_GROUP_H__
#define __CLUTTER_BULLET_GROUP_H__



G_BEGIN_DECLS



#define CLUTTER_BULLET_TYPE_GROUP            (clutter_bullet_group_get_type ())
#define CLUTTER_BULLET_GROUP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_BULLET_TYPE_GROUP, ClutterBulletGroup))
#define CLUTTER_BULLET_IS_GROUP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_BULLET_TYPE_GROUP))
#define CLUTTER_BULLET_GROUP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_BULLET_TYPE_GROUP, ClutterBulletGroupClass))
#define CLUTTER_BULLET_IS_GROUP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_BULLET_TYPE_GROUP))
#define CLUTTER_BULLET_GROUP_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_BULLET_TYPE_GROUP, ClutterBulletGroupClass))



typedef struct _ClutterBulletGroup        ClutterBulletGroup;
typedef struct _ClutterBulletGroupClass   ClutterBulletGroupClass;
typedef struct _ClutterBulletGroupPrivate ClutterBulletGroupPrivate;

typedef struct btDynamicsWorld            btDynamicsWorld;



struct _ClutterBulletGroup
{
  ClutterGroup               parent;

  /*< private >*/
  ClutterBulletGroupPrivate *priv;
};



struct _ClutterBulletGroupClass
{
  ClutterGroupClass   parent;

  void              (*start) (ClutterBulletGroup *);

  void              (*stop)  (ClutterBulletGroup *);
};



GType             clutter_bullet_group_get_type  (void);

ClutterActor *    clutter_bullet_group_new       (gdouble             scale);

btDynamicsWorld * clutter_bullet_group_get_world (ClutterBulletGroup *self);

gdouble           clutter_bullet_group_get_scale (ClutterBulletGroup *self);

void              clutter_bullet_group_bind      (ClutterBulletGroup *self,
                                                  ClutterActor       *actor);

void              clutter_bullet_group_start     (ClutterBulletGroup *self);

void              clutter_bullet_group_stop      (ClutterBulletGroup *self);



G_END_DECLS



#endif /* __CLUTTER_BULLET_GROUP_H__ */
