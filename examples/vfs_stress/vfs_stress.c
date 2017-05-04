#include <stdio.h>
#include <mutek/printk.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <vfs/vfs.h>

#include <libvfs/fs/ramfs/ramfs.h>

#include "my_rand.h"

#define ACTIONS 128

uint32_t bla;

typedef void (action_t)();

extern action_t * const actions[];

static void post_print(struct vfs_node_s *node)
{
    struct vfs_node_s *parent = vfs_node_get_parent(node);
	if ( parent ) { 
        if ( parent != node )
            post_print(parent);
		printk("/");
        vfs_node_refdrop(parent);
	}
	printk("%s", node->name);
}

void random_vfs_actions()
{
	uint16_t count;
	my_rand_init(++bla + (uint32_t)pthread_self());

	uint16_t actions_tab_size = 0;
	action_t * const *act = &actions[0];
	for ( ; *act; ++act )
		++actions_tab_size;

	for ( count = 0; count < ACTIONS; ++count ) {
		uint16_t r = my_rand() % actions_tab_size;

		actions[r]();
//        vfs_dump(vfs_get_root());
	}
    printk("%p Pwd at end: ", pthread_self());
    post_print(vfs_get_cwd());
    printk("\n");
}
