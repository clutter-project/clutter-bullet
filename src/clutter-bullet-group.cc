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



#include "clutter-bullet-group.h"

#include <btBulletDynamicsCommon.h>



#define CLUTTER_BULLET_GROUP_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), CLUTTER_BULLET_TYPE_GROUP, ClutterBulletGroupPrivate))



struct _ClutterBulletGroupPrivate
{
  btDynamicsWorld          *world;
  btConstraintSolver       *solver;
  btCollisionConfiguration *config;

  ClutterTimeline          *clock;
  GTimeVal                  time;
  gint                      steps;
  gdouble                   step;
};



G_DEFINE_TYPE (ClutterBulletGroup, clutter_bullet_group, CLUTTER_TYPE_GROUP);



static void clutter_bullet_group_update   (ClutterTimeline *clock,
                                           gint             msecs,
                                           gpointer         data);

static void clutter_bullet_group_finalize (GObject *obj);



static void
clutter_bullet_group_init (ClutterBulletGroup *self)
{
  self->priv         = CLUTTER_BULLET_GROUP_GET_PRIVATE (self);
  self->priv->config = new btDefaultCollisionConfiguration ();

  btDispatcher          *dispatch   = new btCollisionDispatcher (self->priv->config);
  btBroadphaseInterface *broadphase = new btDbvtBroadphase ();

  self->priv->solver = NULL;
  self->priv->world  = new btDiscreteDynamicsWorld (dispatch, broadphase, self->priv->solver, self->priv->config);

  self->priv->time.tv_sec  = 0;
  self->priv->time.tv_usec = 0;
  self->priv->steps        = 4;
  self->priv->step         = 1 / 60.0;

  self->priv->clock = clutter_timeline_new (1000);
  clutter_timeline_set_loop (self->priv->clock, TRUE);
  g_signal_connect (self->priv->clock, "new-frame", G_CALLBACK (clutter_bullet_group_update), self);
}



static void
clutter_bullet_group_class_init (ClutterBulletGroupClass *klass)
{
  GObjectClass *glass = G_OBJECT_CLASS (klass);

  glass->finalize = clutter_bullet_group_finalize;

  g_type_class_add_private (klass, sizeof (ClutterBulletGroupPrivate));
}



static void
clutter_bullet_group_update (ClutterTimeline *clock,
                             gint             msecs,
                             gpointer         data)
{
  ClutterBulletGroup *group = (ClutterBulletGroup *) data;

  gdouble  delta = 0;
  GTimeVal time;

  g_get_current_time (&time);

  if (group->priv->time.tv_sec || group->priv->time.tv_usec)
    delta = 1E-6 * (time.tv_usec - group->priv->time.tv_usec)
          +        (time.tv_sec  - group->priv->time.tv_sec);

  group->priv->time = time;

  group->priv->world->stepSimulation (delta, group->priv->steps, group->priv->step);
}



static void
clutter_bullet_group_finalize (GObject *obj)
{
  ClutterBulletGroup *group = CLUTTER_BULLET_GROUP (obj);

  btDispatcher          *dispatch   = group->priv->world->getDispatcher ();
  btBroadphaseInterface *broadphase = group->priv->world->getBroadphase ();

  delete group->priv->world;
  delete group->priv->solver;
  delete broadphase;
  delete dispatch;
  delete group->priv->config;

  G_OBJECT_CLASS (clutter_bullet_group_parent_class)->finalize (obj);
}
