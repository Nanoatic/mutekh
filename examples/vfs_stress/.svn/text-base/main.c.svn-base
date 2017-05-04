#include <stdio.h>
#include <mutek/printk.h>
#include <mutek/mem_alloc.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <vfs/vfs.h>

#include <libvfs/fs/ramfs/ramfs.h>

#define NTHREAD 10

void random_vfs_actions();

static
struct vfs_node_s * vfs_init()
{
	struct vfs_fs_s *root_fs;

    printk("init vfs... ");
	ramfs_open(&root_fs);
    printk("ok\n");

    struct vfs_node_s *root_node;
    vfs_create_root(root_fs, &root_node);

	return root_node;
}

pthread_t thread[NTHREAD];
pthread_t main_thread;

void *vfs_stress(void *root_ptr)
{
	random_vfs_actions();

    // Reset all before thread deletion...
	vfs_set_cwd(NULL);
	return NULL;
}

void action_rmrf_inner(struct vfs_node_s *_cwd, const char *name);
error_t memory_allocator_stats(struct memory_allocator_region_s *region,
                               size_t *alloc_blocks,
                               size_t *free_size,
                               size_t *free_blocks);

void print_malloc_stats()
{
    size_t alloc_blocks;
    size_t free_size;
    size_t free_blocks;

    memory_allocator_stats(default_region,
                           &alloc_blocks, &free_size, &free_blocks);

    printk("Malloc stats: block: alloc: %d free: %d; %d bytes left\n",
           alloc_blocks, free_blocks, free_size);
}

void *_main(void *root_ptr)
{
	struct vfs_node_s *root = root_ptr;

    print_malloc_stats();

    size_t alloc;
    memory_allocator_stats(default_region,
                           &alloc, 0, 0);

	while (1) {
		size_t i;
		for ( i=0; i<NTHREAD; ++i )
			pthread_create(&thread[i], NULL, vfs_stress, root);
		for ( i=0; i<NTHREAD; ++i )
			pthread_join(thread[i], NULL);

        print_malloc_stats();

		printk("Tree:\n");
		vfs_dump(root);
		vfs_dump_lru(root);
		ramfs_dump(root->fs);

		printk("Cleaning up /...\n");
        action_rmrf_inner(root, ".");

		printk("Tree:\n");
		vfs_dump(root);
		ramfs_dump(root->fs);

		printk("System still alive after all this...\n");
        print_malloc_stats();

		for ( i=0; i<100000; ++i )
			asm volatile("nop");
//        memory_allocator_dump_used(default_region, alloc);
		printk("Going on...\n");
	}
	return NULL;
}

void app_start()
{
	struct vfs_node_s *root = vfs_init();

	pthread_create(&main_thread, NULL, _main, root);
}
