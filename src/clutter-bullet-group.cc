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



#include <clutter/clutter.h>

#include "clutter-bullet-group.h"
#include "clutter-bullet-actor.h"

#include <BulletCollision/CollisionShapes/btBox2dShape.h>
#include <btBulletDynamicsCommon.h>

#include "clutter-bullet-motion-state-private.h"



#define CLUTTER_BULLET_GROUP_INFO_KEY "clutter-bullet-group-info"
#define CLUTTER_BULLET_GROUP_BODY_KEY "clutter-bullet-group-body"

#define CLUTTER_BULLET_GROUP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_BULLET_TYPE_GROUP, ClutterBulletGroupPrivate))



typedef struct _ClutterBulletGroupInfo ClutterBulletGroupInfo;

typedef struct _ClutterBulletGroupBody ClutterBulletGroupBody;



struct _ClutterBulletGroupPrivate
{
  btDynamicsWorld          *world;
  btConstraintSolver       *solver;
  btCollisionConfiguration *config;

  guint                     timer;
  GTimeVal                  time;
  gint                      steps;
  gdouble                   step;

  gdouble                   scale;
};



struct _ClutterBulletGroupInfo
{
  ClutterBulletGroup *group;

  gpointer            object;
  gulong              signal;
};



struct _ClutterBulletGroupBody
{
  ClutterBulletGroup *group;

  btRigidBody        *body;
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

static void     clutter_bullet_group_real_bind    (GObject               *obj,
                                                   GParamSpec            *spec,
                                                   gpointer               data);

static void     clutter_bullet_group_detach_info  (gpointer               data);

static void     clutter_bullet_group_detach_body  (gpointer               data);

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
)



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
      g_object_notify (obj, "scale");
      break;

    case PROP_GRAVITY:
      {
        const ClutterVertex *vector = (const ClutterVertex *) g_value_get_boxed (val);

        if (vector != NULL)
        {
          btVector3 gravity (vector->x, vector->y, vector->z);

          gravity /= self->priv->scale;

          self->priv->world->setGravity (gravity);
          g_object_notify (obj, "gravity");
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



gdouble
clutter_bullet_group_get_scale (ClutterBulletGroup *self)
{
  return self->priv->scale;
}



btRigidBody *
clutter_bullet_group_get_body (ClutterBulletGroup *self,
                               ClutterActor       *actor)
{
  btRigidBody *body = NULL;

  g_return_val_if_fail (CLUTTER_IS_ACTOR (actor), NULL);

  if (CLUTTER_BULLET_IS_ACTOR (actor))
  {
    ClutterBulletActor *shell = CLUTTER_BULLET_ACTOR (actor);

    if ((body = clutter_bullet_actor_get_body (shell)) == NULL)
    {
      ClutterActor *child = clutter_bullet_actor_get_actor (shell);

      if (CLUTTER_BULLET_IS_GROUP (self) && clutter_actor_has_allocation (child))
      {
        /*
         * there's no real reason this should happen
         * but if it does, we might loop indefinitely
         * depending on what notify::body handlers do
         * in almost all cases, it won't loop forever
         */

        g_warning ("ClutterBulletActor notify::body imminent");

        clutter_bullet_group_bind (self, actor);

        body = clutter_bullet_actor_get_body (shell);
      }
    }
  }
  else
  {
    GObject                *obj;
    const gchar            *key;
    ClutterBulletGroupBody *data;

    obj  = G_OBJECT (actor);
    key  = CLUTTER_BULLET_GROUP_BODY_KEY;
    data = (ClutterBulletGroupBody *) g_object_get_data (obj, key);

    if (data == NULL && CLUTTER_BULLET_IS_GROUP (self) && clutter_actor_has_allocation (actor))
    {
      clutter_bullet_group_bind (self, actor);

      data = (ClutterBulletGroupBody *) g_object_get_data (obj, key);
    }

    if (data != NULL)
      body = data->body;
  }

  return body;
}



static void
clutter_bullet_group_add (ClutterContainer *self,
                          ClutterActor     *actor)
{
  ClutterActor *parent = NULL;
  ClutterActor *child  = actor;

  if (CLUTTER_BULLET_IS_ACTOR (actor))
  {
    ClutterBulletActor *shell = CLUTTER_BULLET_ACTOR (actor);

    child = clutter_bullet_actor_get_actor (shell);

    if (child == NULL)
      child = actor;
  }

  parent = clutter_actor_get_parent (child);
  parent_container->add (self, child);

  if (parent == NULL)
  {
    ClutterBulletGroupInfo *info = g_new (ClutterBulletGroupInfo, 1);
    info->group  = CLUTTER_BULLET_GROUP (self);
    info->object = child;
    info->signal = 0;

    g_object_set_data_full (G_OBJECT (actor),
                            CLUTTER_BULLET_GROUP_INFO_KEY,
                            info, clutter_bullet_group_detach_info);

    if (clutter_actor_has_allocation (child))
    {
      const gchar  *name  = "allocation";
      GObjectClass *klass = G_OBJECT_GET_CLASS (child);
      GParamSpec   *spec  = g_object_class_find_property (klass, name);

      clutter_bullet_group_real_bind (G_OBJECT (child), spec, actor);
    }
    else
    {
      const gchar *name = "notify::allocation";
      GCallback    call = G_CALLBACK (clutter_bullet_group_real_bind);

      info->signal = g_signal_connect (child, name, call, actor);
    }
  }
}



void
clutter_bullet_group_bind (ClutterBulletGroup *self,
                           ClutterActor       *actor)
{
  ClutterActor *child = actor;

  if (CLUTTER_BULLET_IS_ACTOR (actor))
  {
    ClutterBulletActor *shell = CLUTTER_BULLET_ACTOR (actor);

    child = clutter_bullet_actor_get_actor (shell);

    if (child == NULL)
      child = actor;
  }

  const gchar  *name  = "allocation";
  GObjectClass *klass = G_OBJECT_GET_CLASS (child);
  GParamSpec   *spec  = g_object_class_find_property (klass, name);

  clutter_bullet_group_real_bind (G_OBJECT (child), spec, actor);
}



static void
clutter_bullet_group_real_bind (GObject    *obj,
                                GParamSpec *spec,
                                gpointer    data)
{
  ClutterActor *child = CLUTTER_ACTOR (obj);
  ClutterActor *actor = CLUTTER_ACTOR (data);
  const gchar  *key   = CLUTTER_BULLET_GROUP_INFO_KEY;
  gpointer      ptr   = g_object_get_data (G_OBJECT (actor), key);

  if (ptr == NULL || !clutter_actor_has_allocation (child))
    return;

  ClutterBulletGroupInfo *info = (ClutterBulletGroupInfo *) ptr;
  ClutterBulletGroup     *self = info->group;

  g_object_set_data (G_OBJECT (actor), key, NULL);

  if (CLUTTER_BULLET_IS_ACTOR (actor))
    clutter_bullet_actor_bind (CLUTTER_BULLET_ACTOR (actor), self);
  else
  {
    ClutterBulletGroupBody *body;
    btCollisionShape       *shape;
    btVector3               tensor;
    gfloat                  w, h;

    clutter_actor_get_size (child, &w, &h);

    w /= self->priv->scale;
    h /= self->priv->scale;

    shape = new btBox2dShape (btVector3 (w / 2, h / 2, 0));

    shape->setMargin (0);
    shape->calculateLocalInertia (0, tensor);

    body        = new ClutterBulletGroupBody;
    body->group = self;
    body->body  = new btRigidBody (
      btRigidBody::btRigidBodyConstructionInfo (
        0,
        new ClutterBulletMotionState (child, self->priv->scale),
        shape,
        tensor
      )
    );

    self->priv->world->addRigidBody (body->body);

    g_object_set_data_full (G_OBJECT (actor),
                            CLUTTER_BULLET_GROUP_BODY_KEY,
                            body, clutter_bullet_group_detach_body);
  }
}



static void
clutter_bullet_group_detach_info (gpointer data)
{
  ClutterBulletGroupInfo *info = (ClutterBulletGroupInfo *) data;

  if (info->signal)
    g_signal_handler_disconnect (info->object, info->signal);

  g_free (info);
}



static void
clutter_bullet_group_detach_body (gpointer data)
{
  ClutterBulletGroupBody *body = (ClutterBulletGroupBody *) data;

  body->group->priv->world->removeRigidBody (body->body);

  delete body->body->getCollisionShape ();
  delete body->body->getMotionState ();
  delete body->body;
  delete body;
}



static void
clutter_bullet_group_remove (ClutterContainer *self,
                             ClutterActor     *actor)
{
  ClutterActor *child = actor;
  const gchar  *body  = CLUTTER_BULLET_GROUP_BODY_KEY;
  const gchar  *info  = CLUTTER_BULLET_GROUP_INFO_KEY;

  g_object_set_data (G_OBJECT (actor), body, NULL);
  g_object_set_data (G_OBJECT (actor), info, NULL);

  if (CLUTTER_BULLET_IS_ACTOR (actor))
  {
    ClutterBulletGroup *group = CLUTTER_BULLET_GROUP (self);
    ClutterBulletActor *shell = CLUTTER_BULLET_ACTOR (actor);

    child = clutter_bullet_actor_get_actor (shell);

    if (child == NULL)
      child = actor;

    if (clutter_actor_get_parent (child) != NULL)
      clutter_bullet_actor_unbind (shell, group);
  }

  parent_container->remove (self, child);
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
  ClutterBulletGroup *self  = CLUTTER_BULLET_GROUP (data);
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

  btDispatcher          *dispatch   = self->priv->world->getDispatcher ();
  btBroadphaseInterface *broadphase = self->priv->world->getBroadphase ();

  delete self->priv->world;
  delete self->priv->solver;
  delete broadphase;
  delete dispatch;
  delete self->priv->config;

  G_OBJECT_CLASS (clutter_bullet_group_parent_class)->finalize (obj);
}
