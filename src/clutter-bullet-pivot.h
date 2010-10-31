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



#ifndef __CLUTTER_BULLET_PIVOT_H__
#define __CLUTTER_BULLET_PIVOT_H__



#include "clutter-bullet-joint.h"



G_BEGIN_DECLS



#define CLUTTER_BULLET_TYPE_PIVOT            (clutter_bullet_pivot_get_type ())
#define CLUTTER_BULLET_PIVOT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_BULLET_TYPE_PIVOT, ClutterBulletPivot))
#define CLUTTER_BULLET_IS_PIVOT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_BULLET_TYPE_PIVOT))
#define CLUTTER_BULLET_PIVOT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_BULLET_TYPE_PIVOT, ClutterBulletPivotClass))
#define CLUTTER_BULLET_IS_PIVOT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_BULLET_TYPE_PIVOT))
#define CLUTTER_BULLET_PIVOT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_BULLET_TYPE_PIVOT, ClutterBulletPivotClass))



typedef struct _ClutterBulletPivot        ClutterBulletPivot;
typedef struct _ClutterBulletPivotClass   ClutterBulletPivotClass;
typedef struct _ClutterBulletPivotPrivate ClutterBulletPivotPrivate;



struct _ClutterBulletPivot
{
  ClutterBulletJoint         parent;

  /*< private >*/
  ClutterBulletPivotPrivate *priv;
};



struct _ClutterBulletPivotClass
{
  ClutterBulletJointClass parent;
};



GType                clutter_bullet_pivot_get_type (void);

ClutterBulletJoint * clutter_bullet_pivot_new      (ClutterActor  *actor_0,
                                                    ClutterVertex *pivot_0,
                                                    ClutterActor  *actor_1,
                                                    ClutterVertex *pivot_1);



G_END_DECLS



#endif /* __CLUTTER_BULLET_PIVOT_H__ */
