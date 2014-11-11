/*
 * Copyright (C) 2013-2014 Kay Sievers
 * Copyright (C) 2013-2014 Greg Kroah-Hartman <gregkh@linuxfoundation.org>
 * Copyright (C) 2013-2014 Daniel Mack <daniel@zonque.org>
 * Copyright (C) 2013-2014 David Herrmann <dh.herrmann@gmail.com>
 * Copyright (C) 2013-2014 Linux Foundation
 *
 * kdbus is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 */

#ifndef __KDBUS_NODE_H
#define __KDBUS_NODE_H

#include <linux/atomic.h>
#include <linux/kernel.h>
#include <linux/lockdep.h>
#include <linux/mutex.h>
#include <linux/wait.h>

struct kdbus_domain;
struct kdbus_bus;
struct kdbus_ep;

enum kdbus_node_type {
	KDBUS_NODE_DOMAIN,
	KDBUS_NODE_BUS,
	KDBUS_NODE_ENDPOINT,
};

struct kdbus_node {
	atomic_t refcnt;
	struct mutex lock;
	unsigned int id;
	unsigned int type;

	wait_queue_head_t waitq;
	atomic_t active;
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map dep_map;
#endif
};

typedef void (*kdbus_node_free_t) (struct kdbus_node *node);
typedef void (*kdbus_node_release_t) (struct kdbus_node *node);

void kdbus_init_nodes(void);
void kdbus_exit_nodes(void);

int kdbus_node_init(struct kdbus_node *node, unsigned int type);
struct kdbus_node *kdbus_node_ref(struct kdbus_node *node);
struct kdbus_node *kdbus_node_unref(struct kdbus_node *node,
				    kdbus_node_free_t free_cb);

bool kdbus_node_is_active(struct kdbus_node *node);
void kdbus_node_activate(struct kdbus_node *node);
void kdbus_node_deactivate(struct kdbus_node *node,
			   kdbus_node_release_t release_cb);
void kdbus_node_drain(struct kdbus_node *node);

bool kdbus_node_acquire(struct kdbus_node *node);
void kdbus_node_release(struct kdbus_node *node,
			kdbus_node_release_t release_cb);

struct kdbus_node *kdbus_node_find_by_id(unsigned int id);

#endif