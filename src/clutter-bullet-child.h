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



#ifndef __CLUTTER_BULLET_CHILD_H__
#define __CLUTTER_BULLET_CHILD_H__



#define CLUTTER_BULLET_TYPE_CHILD            (clutter_bullet_child_get_type ())
#define CLUTTER_BULLET_CHILD(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_BULLET_TYPE_CHILD, ClutterBulletChild))
#define CLUTTER_BULLET_IS_CHILD(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_BULLET_TYPE_CHILD))
#define CLUTTER_BULLET_CHILD_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_BULLET_TYPE_CHILD, ClutterBulletChildClass))
#define CLUTTER_BULLET_IS_CHILD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_BULLET_TYPE_CHILD))
#define CLUTTER_BULLET_CHILD_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_BULLET_TYPE_CHILD, ClutterBulletChildClass))



typedef struct _ClutterBulletChild        ClutterBulletChild;
typedef struct _ClutterBulletChildClass   ClutterBulletChildClass;
typedef struct _ClutterBulletChildPrivate ClutterBulletChildPrivate;



struct _ClutterBulletChild
{
  ClutterChildMeta           parent;

  /*< private >*/
  ClutterBulletChildPrivate *priv;
};



struct _ClutterBulletChildClass
{
  ClutterChildMetaClass parent;
};



GType clutter_bullet_child_get_type (void);



#endif /* __CLUTTER_BULLET_CHILD_H__ */
