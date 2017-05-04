#include <stdio.h>
#include <mutek/printk.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <vfs/vfs.h>

#include "my_rand.h"

//#define dprintk(...) do{}while(0)
#define dprintk(...) printk(__VA_ARGS__)

typedef void (action_t)();

static const char chtab[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_0123456789.";

static void create_random_name(char *s, size_t size)
{
    memset(s, 0, size);
    --size;
	size = (my_rand() % size) + 1;
	size_t i;

	for ( i=0; i<size; ++i ) {
		s[i] = chtab[my_rand() % (sizeof(chtab)-1)];
	}
	s[i] = 0;
}

static error_t get_random_name(struct vfs_node_s *base, char *name)
{
    struct vfs_file_s *dir;
    struct vfs_dirent_s dirent;
    struct vfs_stat_s stat;
    bool_t done = 0;

	memset(name, 0, CONFIG_VFS_NAMELEN);

	error_t err = vfs_stat(vfs_get_root(), vfs_get_cwd(), ".", &stat);
	if ( ! stat.size )
		return -EIO;

	size_t n = my_rand() % stat.size;

	err = vfs_open(vfs_get_root(), base, ".",
				   VFS_OPEN_READ | VFS_OPEN_DIR, &dir);
    if (err) {
		return -EIO;
	}

    for (;;) {
        ssize_t rlen = vfs_file_read(dir, &dirent, sizeof(dirent));
        if ( rlen != sizeof(dirent) )
            break;
        done = 1;
//        printk("readdir %d: %s\n", n, dirent.name);
        if ( n == 0 )
            break;
        --n;
    }
    if ( done )
        memcpy(name, dirent.name, CONFIG_VFS_NAMELEN);

    vfs_file_close(dir);
	return done ? 0 : -EUNKNOWN;
}

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

void action_cwd()
{
	struct vfs_node_s *node = NULL;
	struct vfs_node_s *base = vfs_get_cwd();
	error_t err;
	char name[CONFIG_VFS_NAMELEN];

	if ( my_rand() > 0xd000 ) {
		name[0] = '.';
		name[1] = '.';
        name[2] = 0;
		err = 0;
	} else {
		err = get_random_name(base, name);
		if ( err ) {
			err = 0;
			name[0] = '/';
			name[1] = 0;
		}
	}

	dprintk("%s \"%s\"...\n", __FUNCTION__, name);

	err = vfs_lookup(vfs_get_root(), base, name, &node);
	if ( err )
		return;

    assert(node);

    struct vfs_stat_s stat;
    vfs_node_stat(node, &stat);
	if ( stat.type == VFS_NODE_DIR ) {
        dprintk("%p: cwd from ", pthread_self());
        post_print(vfs_get_cwd());
        dprintk(" to ");
        post_print(node);
        dprintk("\n");
		vfs_set_cwd(node);
    }
	vfs_node_refdrop(node);
}

void action_mkdir()
{
	char name[CONFIG_VFS_NAMELEN];
	create_random_name(name, CONFIG_VFS_NAMELEN);

	dprintk("%s \"%s\"...\n", __FUNCTION__, name);

	struct vfs_node_s *node = NULL;
	error_t err = vfs_create(vfs_get_root(), vfs_get_cwd(),
							 name, VFS_NODE_DIR, &node);
	if (err == 0) {
		vfs_node_refdrop(node);
	} else {
		dprintk("%s error %s\n", __FUNCTION__, strerror(err));
	}
}

void action_create_file()
{
	char name[CONFIG_VFS_NAMELEN];
	create_random_name(name, CONFIG_VFS_NAMELEN);

	dprintk("%s \"%s\"...\n", __FUNCTION__, name);

	struct vfs_file_s *file = NULL;
	error_t err = vfs_open(vfs_get_root(), vfs_get_cwd(),
						   name, VFS_OPEN_WRITE|VFS_OPEN_CREATE, &file);
	if (err)
		goto error_open;

	vfs_file_write(file, action_create_file, 128);
	vfs_file_close(file);
	return;

  error_open:
	dprintk("%s error %s\n", __FUNCTION__, strerror(err));
}

void action_rm()
{
	char name[CONFIG_VFS_NAMELEN];
	error_t err = get_random_name(vfs_get_cwd(), name);

	dprintk("%s \"%s\"...\n", __FUNCTION__, name);

	if ( !err )
		vfs_unlink(vfs_get_root(), vfs_get_cwd(), name);
}

error_t action_rmrf_inner(struct vfs_node_s *_cwd, const char *name)
{
	struct vfs_node_s *cwd = vfs_node_refnew(_cwd);
	struct vfs_stat_s stat = {0};
	error_t err;

	err = vfs_stat(vfs_get_root(), cwd, name, &stat);
    dprintk("rmrf stat 'name': %s\n", name, strerror(err));
	if ( err )
		goto end;

	if ( stat.type == VFS_NODE_DIR ) {
        dprintk(" is directory\n");
		struct vfs_node_s *node = NULL;
		if ( vfs_lookup(vfs_get_root(), cwd, name, &node) == 0 ) {
			assert(node);

			while ( 1 ) {
				struct vfs_file_s *dir = NULL;
				struct vfs_dirent_s dirent;
		
				err = vfs_open(vfs_get_root(), node, ".",
							   VFS_OPEN_READ | VFS_OPEN_DIR, &dir);
				if ( err )
					break;
				ssize_t len = vfs_file_read(dir, &dirent, sizeof(dirent));
                dprintk("Read len: %d\n", len);
				vfs_file_close(dir);
				if ( !len )
					break;

				err = action_rmrf_inner(node, dirent.name);
                if ( err )
                    break;
			}

			vfs_node_refdrop(node);
		}
	}
	err = vfs_unlink(vfs_get_root(), cwd, name);
    dprintk(" unlink '%s': %s\n", name, strerror(err));

  end:
	vfs_node_refdrop(cwd);
    return err;
}

void action_rmrf()
{
	char name[CONFIG_VFS_NAMELEN];
	error_t err = get_random_name(vfs_get_cwd(), name);

	dprintk("%s \"%s\"...\n", __FUNCTION__, name);

	if ( !strcmp(name, "..") )
		return;

//	vfs_dump(vfs_get_cwd());

	if ( !err )
		action_rmrf_inner(vfs_get_cwd(), name);
}

void action_mount()
{

}

void action_umount()
{

}

void action_ls()
{
	dprintk("%s...\n", __FUNCTION__);

    struct vfs_file_s *dir = NULL;
    struct vfs_dirent_s dirent;

	error_t err = vfs_open(vfs_get_root(), vfs_get_cwd(), ".",
						   VFS_OPEN_READ | VFS_OPEN_DIR, &dir);
    if (err) {
		dprintk("%s error %s\n", __FUNCTION__, strerror(err));
		return;
	}

    while ( vfs_file_read(dir, &dirent, sizeof(dirent)) == sizeof(dirent) ) {
//        printk("%s [%s] %d\n", dirent.name, dirent.type == VFS_NODE_DIR ? "dir" : "reg", dirent.size);
    }

    vfs_file_close(dir);
}

action_t * const actions[] =
{
	action_cwd,
	action_mkdir,
	action_create_file,
	action_rm,
	action_rmrf,
/* 	action_mount, */
/* 	action_umount, */
	action_ls,
	0,
};

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4
