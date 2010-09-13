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

#include <btBulletDynamicsCommon.h>



#define CLUTTER_BULLET_GROUP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_BULLET_TYPE_GROUP, ClutterBulletGroupPrivate))



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



enum
{
  PROP_0,
  PROP_SCALE
};



static ClutterContainerIface *container;



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

static gboolean clutter_bullet_group_update       (gpointer               data);

static void     clutter_bullet_group_finalize     (GObject               *obj);

static void     clutter_container_iface_init      (ClutterContainerIface *iface,
                                                   gpointer               data);



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

  g_type_class_add_private (klass, sizeof (ClutterBulletGroupPrivate));

  glass->get_property = clutter_bullet_group_get_property;
  glass->set_property = clutter_bullet_group_set_property;
  glass->finalize     = clutter_bullet_group_finalize;

  GParamSpec *spec = g_param_spec_double ("scale",
                                          "Pixels per metre",
                                          "Set pixels per metre",
                                          G_MINDOUBLE,
                                          G_MAXDOUBLE,
                                          1,
                                          (GParamFlags)
                                          (G_PARAM_READABLE |
                                           G_PARAM_CONSTRUCT_ONLY));

  g_object_class_install_property (glass, PROP_SCALE, spec);
}



ClutterActor *
clutter_bullet_group_new (gdouble scale)
{
  return (ClutterActor *) g_object_new (CLUTTER_BULLET_TYPE_GROUP,
                                        "scale", scale,
                                        NULL);
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

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (obj, key, spec);
      break;
  }
}



void
clutter_bullet_group_start (ClutterBulletGroup *group)
{
  if (group->priv->timer)
    return;

  g_get_current_time (&group->priv->time);

  group->priv->timer = clutter_threads_add_repaint_func (clutter_bullet_group_update, group, NULL);
}



void
clutter_bullet_group_stop (ClutterBulletGroup *group)
{
  if (!group->priv->timer)
    return;

  clutter_threads_remove_repaint_func (group->priv->timer);

  group->priv->timer        = 0;
  group->priv->time.tv_sec  = 0;
  group->priv->time.tv_usec = 0;
}



static void
clutter_bullet_group_add (ClutterContainer *self,
                          ClutterActor     *actor)
{
  ClutterBulletGroup *group = CLUTTER_BULLET_GROUP (self);

  container->add (self, actor);

  if (CLUTTER_BULLET_IS_ACTOR (actor))
    clutter_bullet_actor_bind (CLUTTER_BULLET_ACTOR (actor), group);
}



static void
clutter_bullet_group_remove (ClutterContainer *self,
                             ClutterActor     *actor)
{
  ClutterBulletGroup *group = CLUTTER_BULLET_GROUP (self);

  container->remove (self, actor);

  if (CLUTTER_BULLET_IS_ACTOR (actor))
    clutter_bullet_actor_unbind (CLUTTER_BULLET_ACTOR (actor), group);
}



static gboolean
clutter_bullet_group_update (gpointer data)
{
  ClutterBulletGroup *group = (ClutterBulletGroup *) data;
  gdouble             delta = 0;
  GTimeVal            time;

  g_get_current_time (&time);

  if (group->priv->time.tv_sec || group->priv->time.tv_usec)
    delta = 1E-6 * (time.tv_usec - group->priv->time.tv_usec)
          +        (time.tv_sec  - group->priv->time.tv_sec);

  group->priv->time = time;

  group->priv->world->stepSimulation (delta, group->priv->steps, group->priv->step);

  return TRUE;
}



static void
clutter_bullet_group_finalize (GObject *obj)
{
  ClutterBulletGroup *group = CLUTTER_BULLET_GROUP (obj);

  clutter_bullet_group_stop (group);

  btDispatcher          *dispatch   = group->priv->world->getDispatcher ();
  btBroadphaseInterface *broadphase = group->priv->world->getBroadphase ();

  delete group->priv->world;
  delete group->priv->solver;
  delete broadphase;
  delete dispatch;
  delete group->priv->config;

  G_OBJECT_CLASS (clutter_bullet_group_parent_class)->finalize (obj);
}



static void
clutter_container_iface_init (ClutterContainerIface *iface,
                              gpointer               data)
{
  container = (ClutterContainerIface *) g_type_interface_peek_parent (iface);

  iface->add    = clutter_bullet_group_add;
  iface->remove = clutter_bullet_group_remove;
}
