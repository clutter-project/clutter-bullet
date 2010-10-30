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



#ifndef __CLUTTER_BULLET_JOINT_H__
#define __CLUTTER_BULLET_JOINT_H__



#include "clutter-bullet-group.h"



G_BEGIN_DECLS



#define CLUTTER_BULLET_TYPE_JOINT            (clutter_bullet_joint_get_type ())
#define CLUTTER_BULLET_JOINT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_BULLET_TYPE_JOINT, ClutterBulletJoint))
#define CLUTTER_BULLET_IS_JOINT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_BULLET_TYPE_JOINT))
#define CLUTTER_BULLET_JOINT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_BULLET_TYPE_JOINT, ClutterBulletJointClass))
#define CLUTTER_BULLET_IS_JOINT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_BULLET_TYPE_JOINT))
#define CLUTTER_BULLET_JOINT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_BULLET_TYPE_JOINT, ClutterBulletJointClass))



typedef struct _ClutterBulletJoint        ClutterBulletJoint;
typedef struct _ClutterBulletJointClass   ClutterBulletJointClass;
typedef struct _ClutterBulletJointPrivate ClutterBulletJointPrivate;



struct _ClutterBulletJoint
{
  GObject                    parent;

  /*< private >*/
  ClutterBulletJointPrivate *priv;
};



struct _ClutterBulletJointClass
{
  GObjectClass   parent;

  void         (*bind) (ClutterBulletJoint *self,
                        ClutterBulletGroup *group,
                        GSList             *actors);
};



GType clutter_bullet_joint_get_type (void);

void  clutter_bullet_joint_set      (ClutterBulletJoint *self,
                                     ...);

void  clutter_bullet_joint_add      (ClutterBulletJoint *self,
                                     ClutterActor       *actor);

void  clutter_bullet_joint_fix      (ClutterBulletJoint *self);



G_END_DECLS



#endif /* __CLUTTER_BULLET_JOINT_H__ */
