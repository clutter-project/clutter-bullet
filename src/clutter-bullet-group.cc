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

#include "clutter-bullet-group.h"
#include "clutter-bullet-actor.h"

#include <BulletCollision/CollisionShapes/btBox2dShape.h>
#include <btBulletDynamicsCommon.h>

#include "clutter-bullet-private.h"



#define CLUTTER_BULLET_GROUP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_BULLET_TYPE_GROUP, ClutterBulletGroupPrivate))



struct _ClutterBulletGroupPrivate
{
  btDynamicsWorld          *world;
  btConstraintSolver       *solver;
  btCollisionConfiguration *config;

  GHashTable               *body;

  guint                     timer;
  GTimeVal                  time;
  gint                      steps;
  gdouble                   step;

  gdouble                   scale;
};



enum
{
  PROP_0,
  PROP_SCALE,
  PROP_GRAVITY
};



static ClutterContainerIface *parent_container;



static void     clutter_container_iface_init      (ClutterContainerIface *iface,
                                                   gpointer               data);

static void     clutter_bullet_group_get_property (GObject               *obj,
                                                   guint                  key,
                                                   GValue                *val,
                                                   GParamSpec            *spec);

static void     clutter_bullet_group_set_property (GObject               *obj,
                                                   guint                  key,
                                                   const GValue          *val,
                                                   GParamSpec            *spec);

static void     clutter_bullet_group_add          (ClutterContainer      *self,
                                                   ClutterActor          *actor);

static void     clutter_bullet_group_remove       (ClutterContainer      *self,
                                                   ClutterActor          *actor);

static void     clutter_bullet_group_real_start   (ClutterBulletGroup    *self);

static void     clutter_bullet_group_real_stop    (ClutterBulletGroup    *self);

static gboolean clutter_bullet_group_update       (gpointer               data);

static void     clutter_bullet_group_finalize     (GObject               *obj);



G_DEFINE_TYPE_WITH_CODE (
  ClutterBulletGroup,
  clutter_bullet_group,
  CLUTTER_TYPE_GROUP,
  G_IMPLEMENT_INTERFACE (
    CLUTTER_TYPE_CONTAINER,
    clutter_container_iface_init
  )
);



static void
clutter_bullet_group_init (ClutterBulletGroup *self)
{
  self->priv         = CLUTTER_BULLET_GROUP_GET_PRIVATE (self);
  self->priv->config = new btDefaultCollisionConfiguration ();

  btDispatcher          *dispatch   = new btCollisionDispatcher (self->priv->config);
  btBroadphaseInterface *broadphase = new btDbvtBroadphase ();

  self->priv->solver = NULL;
  self->priv->world  = new btDiscreteDynamicsWorld (dispatch, broadphase, self->priv->solver, self->priv->config);
  self->priv->world->setGravity (btVector3 (0, 9.8, 0));

  self->priv->body = g_hash_table_new (NULL, NULL);

  self->priv->timer        = 0;
  self->priv->time.tv_sec  = 0;
  self->priv->time.tv_usec = 0;
  self->priv->steps        = 60;
  self->priv->step         = 1 / 60.0;
}



static void
clutter_bullet_group_class_init (ClutterBulletGroupClass *klass)
{
  GObjectClass *glass = G_OBJECT_CLASS (klass);
  GParamSpec   *spec;

  g_type_class_add_private (klass, sizeof (ClutterBulletGroupPrivate));

  glass->get_property = clutter_bullet_group_get_property;
  glass->set_property = clutter_bullet_group_set_property;
  glass->finalize     = clutter_bullet_group_finalize;
  klass->start        = clutter_bullet_group_real_start;
  klass->stop         = clutter_bullet_group_real_stop;

  spec = g_param_spec_double ("scale",
                              "Scaling factor",
                              "Scale in pixels per metre",
                              G_MINDOUBLE,
                              G_MAXDOUBLE,
                              1,
                              (GParamFlags) (G_PARAM_READWRITE |
                                             G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (glass, PROP_SCALE, spec);

  spec = g_param_spec_boxed ("gravity",
                             "Gravity vector",
                             "Gravity in pixels per square second",
                             CLUTTER_TYPE_VERTEX,
                             (GParamFlags) (G_PARAM_READWRITE |
                                            G_PARAM_CONSTRUCT));

  g_object_class_install_property (glass, PROP_GRAVITY, spec);
}



static void
clutter_container_iface_init (ClutterContainerIface *iface,
                              gpointer               data)
{
  parent_container = (ClutterContainerIface *) g_type_interface_peek_parent (iface);

  iface->add    = clutter_bullet_group_add;
  iface->remove = clutter_bullet_group_remove;
}



ClutterActor *
clutter_bullet_group_new (gdouble scale)
{
  return CLUTTER_ACTOR (g_object_new (CLUTTER_BULLET_TYPE_GROUP,
                                      "scale", scale,
                                      NULL));
}



static void
clutter_bullet_group_get_property (GObject    *obj,
                                   guint       key,
                                   GValue     *val,
                                   GParamSpec *spec)
{
  ClutterBulletGroup *self = CLUTTER_BULLET_GROUP (obj);

  switch (key)
  {
    case PROP_SCALE:
      g_value_set_double (val, self->priv->scale);
      break;

    case PROP_GRAVITY:
      {
        ClutterVertex vector;
        btVector3     gravity = self->priv->world->getGravity ();

        gravity *= self->priv->scale;

        vector.x = gravity.x ();
        vector.y = gravity.y ();
        vector.z = gravity.z ();

        g_value_set_boxed (val, &vector);
      }

      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, key, spec);
      break;
  }
}



static void
clutter_bullet_group_set_property (GObject      *obj,
                                   guint         key,
                                   const GValue *val,
                                   GParamSpec   *spec)
{
  ClutterBulletGroup *self = CLUTTER_BULLET_GROUP (obj);

  switch (key)
  {
    case PROP_SCALE:
      self->priv->scale = g_value_get_double (val);
      break;

    case PROP_GRAVITY:
      {
        const ClutterVertex *vector = (const ClutterVertex *) g_value_get_boxed (val);

        if (vector != NULL)
        {
          btVector3 gravity (vector->x, vector->y, vector->z);

          gravity /= self->priv->scale;

          self->priv->world->setGravity (gravity);
        }
      }

      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, key, spec);
      break;
  }
}



btDynamicsWorld *
clutter_bullet_group_get_world (ClutterBulletGroup *self)
{
  return self->priv->world;
}



static void
clutter_bullet_group_add (ClutterContainer *self,
                          ClutterActor     *actor)
{
  ClutterBulletGroup *group = CLUTTER_BULLET_GROUP (self);

  if (CLUTTER_BULLET_IS_ACTOR (actor))
  {
    ClutterBulletActor *binder = CLUTTER_BULLET_ACTOR (actor);
    ClutterActor       *child  = clutter_bullet_actor_get_actor (binder);

    parent_container->add (self, child == NULL ? actor : child);

    clutter_bullet_actor_bind (binder, group);
  }
  else
  {
    parent_container->add (self, actor);

    if (g_hash_table_lookup (group->priv->body, actor) == NULL)
    {
      btCollisionShape *shape;
      btVector3         tensor;
      gfloat            w, h;

      clutter_actor_get_size (actor, &w, &h);

      w /= group->priv->scale;
      h /= group->priv->scale;

      shape = new btBox2dShape (btVector3 (w / 2, h / 2, 0));

      shape->setMargin (0);
      shape->calculateLocalInertia (0, tensor);

      btRigidBody *body = new btRigidBody (
        btRigidBody::btRigidBodyConstructionInfo (
          0,
          new ClutterBulletMotionState (actor, group->priv->scale),
          shape,
          tensor
        )
      );

      group->priv->world->addRigidBody (body);

      g_hash_table_replace (group->priv->body, actor, body);
    }
  }
}



static void
clutter_bullet_group_remove (ClutterContainer *self,
                             ClutterActor     *actor)
{
  ClutterBulletGroup *group = CLUTTER_BULLET_GROUP (self);

  if (CLUTTER_BULLET_IS_ACTOR (actor))
  {
    ClutterBulletActor *binder = CLUTTER_BULLET_ACTOR (actor);
    ClutterActor       *child  = clutter_bullet_actor_get_actor (binder);

    clutter_bullet_actor_unbind (binder, group);

    parent_container->remove (self, child == NULL ? actor : child);
  }
  else
  {
    gpointer     obj  = g_hash_table_lookup (group->priv->body, actor);
    btRigidBody *body = (btRigidBody *) obj;

    if (body != NULL)
    {
      g_hash_table_remove (group->priv->body, actor);

      group->priv->world->removeRigidBody (body);

      delete body->getCollisionShape ();
      delete body->getMotionState ();
      delete body;
    }

    parent_container->remove (self, actor);
  }
}



void
clutter_bullet_group_start (ClutterBulletGroup *self)
{
  CLUTTER_BULLET_GROUP_GET_CLASS (self)->start (self);
}



static void
clutter_bullet_group_real_start (ClutterBulletGroup *self)
{
  if (self->priv->timer)
    return;

  g_get_current_time (&self->priv->time);

  self->priv->timer = clutter_frame_source_add (60, clutter_bullet_group_update, self);
}



void
clutter_bullet_group_stop (ClutterBulletGroup *self)
{
  CLUTTER_BULLET_GROUP_GET_CLASS (self)->stop (self);
}



static void
clutter_bullet_group_real_stop (ClutterBulletGroup *self)
{
  if (!self->priv->timer)
    return;

  self->priv->timer        = 0;
  self->priv->time.tv_sec  = 0;
  self->priv->time.tv_usec = 0;
}



static gboolean
clutter_bullet_group_update (gpointer data)
{
  ClutterBulletGroup *self  = (ClutterBulletGroup *) data;
  gdouble             delta = 0;
  GTimeVal            time;

  if (!self->priv->timer)
    return FALSE;

  g_get_current_time (&time);

  if (self->priv->time.tv_sec || self->priv->time.tv_usec)
    delta = 1E-6 * (time.tv_usec - self->priv->time.tv_usec)
          +        (time.tv_sec  - self->priv->time.tv_sec);

  self->priv->time = time;

  self->priv->world->stepSimulation (delta, self->priv->steps, self->priv->step);

  return TRUE;
}



static void
clutter_bullet_group_finalize (GObject *obj)
{
  ClutterBulletGroup *self = CLUTTER_BULLET_GROUP (obj);

  clutter_bullet_group_stop (self);

  clutter_group_remove_all (CLUTTER_GROUP (obj));

  g_hash_table_destroy (self->priv->body);

  btDispatcher          *dispatch   = self->priv->world->getDispatcher ();
  btBroadphaseInterface *broadphase = self->priv->world->getBroadphase ();

  delete self->priv->world;
  delete self->priv->solver;
  delete broadphase;
  delete dispatch;
  delete self->priv->config;

  G_OBJECT_CLASS (clutter_bullet_group_parent_class)->finalize (obj);
}
