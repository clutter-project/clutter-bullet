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
 * SECTION:clutter-bullet-card
 * @short_description: Wrapper for flat rectangular actors
 * @title: ClutterBulletCard
 *
 * #ClutterBulletCard is a wrapper that treats #ClutterActor<!-- -->s
 * as flat rectangular objects when simulated.  It is not unlike how
 * ordinary #ClutterActor<!-- -->s are treated when inserted into a
 * #ClutterBulletGroup, except that a #ClutterBulletCard can move if
 * it has a positive mass.
 */



#include <clutter/clutter.h>

#include "clutter-bullet-card.h"
#include "clutter-bullet-actor.h"

#include <BulletCollision/CollisionShapes/btBox2dShape.h>
#include <btBulletDynamicsCommon.h>

#include "clutter-bullet-motion-state-private.h"



#define CLUTTER_BULLET_CARD_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_BULLET_TYPE_CARD, ClutterBulletCardPrivate))



struct _ClutterBulletCardPrivate
{
  ClutterActor *actor;

  btRigidBody  *body;

  gdouble       mass;
  gdouble       margin;
};



enum
{
  PROP_0,
  PROP_ACTOR,
  PROP_BODY,
  PROP_MASS,
  PROP_MARGIN
};



static void clutter_bullet_actor_iface_init  (ClutterBulletActorInterface *iface,
                                              gpointer                     data);

static void clutter_bullet_card_get_property (GObject                     *obj,
                                              guint                        key,
                                              GValue                      *val,
                                              GParamSpec                  *spec);

static void clutter_bullet_card_set_property (GObject                     *obj,
                                              guint                        key,
                                              const GValue                *val,
                                              GParamSpec                  *spec);

static void clutter_bullet_card_bind         (ClutterBulletActor          *self,
                                              ClutterBulletGroup          *group);

static void clutter_bullet_card_unbind       (ClutterBulletActor          *self,
                                              ClutterBulletGroup          *group);



G_DEFINE_TYPE_WITH_CODE (
  ClutterBulletCard,
  clutter_bullet_card,
  CLUTTER_TYPE_ACTOR,
  G_IMPLEMENT_INTERFACE (
    CLUTTER_BULLET_TYPE_ACTOR,
    clutter_bullet_actor_iface_init
  )
)



static void
clutter_bullet_card_init (ClutterBulletCard *self)
{
  self->priv = CLUTTER_BULLET_CARD_GET_PRIVATE (self);

  self->priv->actor  = NULL;
  self->priv->body   = NULL;
  self->priv->mass   = 1;
  self->priv->margin = CONVEX_DISTANCE_MARGIN;
}



static void
clutter_bullet_card_class_init (ClutterBulletCardClass *klass)
{
  GObjectClass *glass = G_OBJECT_CLASS (klass);
  GParamSpec   *spec;

  g_type_class_add_private (klass, sizeof (ClutterBulletCardPrivate));

  glass->get_property = clutter_bullet_card_get_property;
  glass->set_property = clutter_bullet_card_set_property;

  g_object_class_override_property (glass, PROP_ACTOR, "actor");
  g_object_class_override_property (glass, PROP_BODY, "body");

  spec = g_param_spec_double ("mass",
                              "Object mass",
                              "Mass in kilograms",
                              0,
                              G_MAXDOUBLE,
                              1,
                              (GParamFlags) (G_PARAM_READWRITE |
                                             G_PARAM_CONSTRUCT));

  g_object_class_install_property (glass, PROP_MASS, spec);

  spec = g_param_spec_double ("margin",
                              "Object margin",
                              "Penetration allowance in metres",
                              G_MINDOUBLE,
                              G_MAXDOUBLE,
                              CONVEX_DISTANCE_MARGIN,
                              (GParamFlags) (G_PARAM_READWRITE |
                                             G_PARAM_CONSTRUCT));

  g_object_class_install_property (glass, PROP_MARGIN, spec);
}



static void
clutter_bullet_actor_iface_init (ClutterBulletActorInterface *iface,
                                 gpointer                     data)
{
  iface->bind   = clutter_bullet_card_bind;
  iface->unbind = clutter_bullet_card_unbind;
}



ClutterActor *
clutter_bullet_card_new (ClutterActor *actor)
{
  return CLUTTER_ACTOR (g_object_new (CLUTTER_BULLET_TYPE_CARD,
                                      "actor", actor,
                                      NULL));
}



static void
clutter_bullet_card_get_property (GObject    *obj,
                                  guint       key,
                                  GValue     *val,
                                  GParamSpec *spec)
{
  ClutterBulletCard *self = CLUTTER_BULLET_CARD (obj);

  switch (key)
  {
    case PROP_ACTOR:
      g_value_set_object (val, self->priv->actor);
      break;

    case PROP_BODY:
      g_value_set_pointer (val, self->priv->body);
      break;

    case PROP_MASS:
      g_value_set_double (val, clutter_bullet_card_get_mass (self));
      break;

    case PROP_MARGIN:
      g_value_set_double (val, clutter_bullet_card_get_margin (self));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, key, spec);
      break;
  }
}



static void
clutter_bullet_card_set_property (GObject      *obj,
                                  guint         key,
                                  const GValue *val,
                                  GParamSpec   *spec)
{
  ClutterBulletCard *self = CLUTTER_BULLET_CARD (obj);

  switch (key)
  {
    case PROP_ACTOR:
      self->priv->actor = CLUTTER_ACTOR (g_value_get_object (val));
      g_object_notify (obj, "actor");
      break;

    case PROP_BODY:
      self->priv->body = (btRigidBody *) g_value_get_pointer (val);
      g_object_notify (obj, "body");
      break;

    case PROP_MASS:
      clutter_bullet_card_set_mass (self, g_value_get_double (val));
      break;

    case PROP_MARGIN:
      clutter_bullet_card_set_margin (self, g_value_get_double (val));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, key, spec);
      break;
  }
}



gdouble
clutter_bullet_card_get_mass (ClutterBulletCard *self)
{
  return self->priv->mass;
}



void
clutter_bullet_card_set_mass (ClutterBulletCard *self,
                              gdouble            mass)
{
  if (self->priv->body != NULL)
  {
    btCollisionShape *shape;
    btVector3         tensor;
    gdouble           scale;

    shape = self->priv->body->getCollisionShape ();

    if (shape != NULL)
      shape->calculateLocalInertia (mass, tensor);
    else
    {
      scale  = mass / self->priv->mass;
      tensor = self->priv->body->getInvInertiaDiagLocal ();

      if (tensor.x ())
        tensor.setX (scale / tensor.x ());

      if (tensor.y ())
        tensor.setY (scale / tensor.y ());

      if (tensor.z ())
        tensor.setZ (scale / tensor.z ());
    }

    self->priv->body->setMassProps (mass, tensor);
  }

  self->priv->mass = mass;

  g_object_notify (G_OBJECT (self), "mass");
}



gdouble
clutter_bullet_card_get_margin (ClutterBulletCard *self)
{
  return self->priv->margin;
}



void
clutter_bullet_card_set_margin (ClutterBulletCard *self,
                                gdouble            margin)
{
  if (self->priv->body != NULL)
    if (self->priv->body->getCollisionShape () != NULL)
      self->priv->body->getCollisionShape ()->setMargin (margin);

  self->priv->margin = margin;

  g_object_notify (G_OBJECT (self), "margin");
}



static void
clutter_bullet_card_bind (ClutterBulletActor *self,
                          ClutterBulletGroup *group)
{
  ClutterBulletCard *card = CLUTTER_BULLET_CARD (self);

  btCollisionShape *shape;
  btVector3         tensor;
  gdouble           scale;
  gfloat            w, h;

  scale = clutter_bullet_group_get_scale (group);

  clutter_actor_get_size (card->priv->actor, &w, &h);

  w /= scale;
  h /= scale;

  shape = new btBox2dShape (btVector3 (card->priv->margin + w / 2,
                                       card->priv->margin + h / 2,
                                       card->priv->margin));

  shape->setMargin (card->priv->margin);
  shape->calculateLocalInertia (card->priv->mass, tensor);

  card->priv->body = new btRigidBody (
    btRigidBody::btRigidBodyConstructionInfo (
      card->priv->mass,
      new ClutterBulletMotionState (card->priv->actor, scale),
      shape,
      tensor
    )
  );

  card->priv->body->setDamping (0.4, 0.4);
  card->priv->body->setSleepingThresholds (0.01, 0.01);

  clutter_bullet_group_get_world (group)->addRigidBody (card->priv->body);
}



static void
clutter_bullet_card_unbind (ClutterBulletActor *self,
                            ClutterBulletGroup *group)
{
  ClutterBulletCard *card = CLUTTER_BULLET_CARD (self);

  if (card->priv->body != NULL)
  {
    clutter_bullet_group_get_world (group)->removeRigidBody (card->priv->body);

    delete card->priv->body->getCollisionShape ();
    delete card->priv->body->getMotionState ();
    delete card->priv->body;

    card->priv->body = NULL;
  }
}
