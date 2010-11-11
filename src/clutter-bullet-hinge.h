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



#ifndef __CLUTTER_BULLET_HINGE_H__
#define __CLUTTER_BULLET_HINGE_H__



#include "clutter-bullet-joint.h"



G_BEGIN_DECLS



#define CLUTTER_BULLET_TYPE_HINGE            (clutter_bullet_hinge_get_type ())
#define CLUTTER_BULLET_HINGE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_BULLET_TYPE_HINGE, ClutterBulletHinge))
#define CLUTTER_BULLET_IS_HINGE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_BULLET_TYPE_HINGE))
#define CLUTTER_BULLET_HINGE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_BULLET_TYPE_HINGE, ClutterBulletHingeClass))
#define CLUTTER_BULLET_IS_HINGE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_BULLET_TYPE_HINGE))
#define CLUTTER_BULLET_HINGE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_BULLET_TYPE_HINGE, ClutterBulletHingeClass))



typedef struct _ClutterBulletHinge        ClutterBulletHinge;
typedef struct _ClutterBulletHingeClass   ClutterBulletHingeClass;
typedef struct _ClutterBulletHingePrivate ClutterBulletHingePrivate;



struct _ClutterBulletHinge
{
  ClutterBulletJoint         parent;

  /*< private >*/
  ClutterBulletHingePrivate *priv;
};



struct _ClutterBulletHingeClass
{
  ClutterBulletJointClass parent;
};



GType                clutter_bullet_hinge_get_type (void);

ClutterBulletJoint * clutter_bullet_hinge_new      (ClutterActor  *actor_0,
                                                    ClutterVertex *pivot_0_0,
                                                    ClutterVertex *pivot_0_1,
                                                    ClutterActor  *actor_1,
                                                    ClutterVertex *pivot_1_0,
                                                    ClutterVertex *pivot_1_1);



G_END_DECLS



#endif /* __CLUTTER_BULLET_HINGE_H__ */
