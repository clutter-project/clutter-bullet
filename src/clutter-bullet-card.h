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



#ifndef __CLUTTER_BULLET_CARD_H__
#define __CLUTTER_BULLET_CARD_H__



G_BEGIN_DECLS



#define CLUTTER_BULLET_TYPE_CARD            (clutter_bullet_card_get_type ())
#define CLUTTER_BULLET_CARD(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), CLUTTER_BULLET_TYPE_CARD, ClutterBulletCard))
#define CLUTTER_BULLET_IS_CARD(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), CLUTTER_BULLET_TYPE_CARD))
#define CLUTTER_BULLET_CARD_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), CLUTTER_BULLET_TYPE_CARD, ClutterBulletCardClass))
#define CLUTTER_BULLET_IS_CARD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), CLUTTER_BULLET_TYPE_CARD))
#define CLUTTER_BULLET_CARD_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), CLUTTER_BULLET_TYPE_CARD, ClutterBulletCardClass))



typedef struct _ClutterBulletCard        ClutterBulletCard;
typedef struct _ClutterBulletCardClass   ClutterBulletCardClass;
typedef struct _ClutterBulletCardPrivate ClutterBulletCardPrivate;



struct _ClutterBulletCard
{
  ClutterActor              parent;

  /*< private >*/
  ClutterBulletCardPrivate *priv;
};



struct _ClutterBulletCardClass
{
  ClutterActorClass parent;
};



ClutterActor * clutter_bullet_card_new             (ClutterActor *actor,
                                                    gdouble       mass);

ClutterActor * clutter_bullet_card_new_with_margin (ClutterActor *actor,
                                                    gdouble       mass,
                                                    gdouble       margin);



G_END_DECLS



#endif /* __CLUTTER_BULLET_CARD_H__ */
