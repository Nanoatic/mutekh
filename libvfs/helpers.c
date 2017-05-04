/*
  This file is part of MutekH.

  MutekH is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation; version 2.1 of the
  License.
    
  MutekH is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
    
  You should have received a copy of the GNU Lesser General Public
  License along with MutekH; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301 USA

  Copyright Nicolas Pouillon, <nipo@ssji.net>, 2009
*/

#include <vfs/vfs.h>
#include "vfs-private.h"
#include <mutek/printk.h>

static const char *next_nonslash(const char *str)
{
	while ( *str && *str == '/' )
		str++;
	return str;
}

static const char *next_slash(const char *str)
{
	while ( *str && *str != '/' )
		str++;
	return str;
}

static const char *prev_slash(const char *start, const char *str)
{
	const char *p = str;

	while ( p > start && *(p-1) != '/' )
        --p;
    if ( p > start )
        return p-1;
    return start;
}

#if 0
static const char *prev_token(const char *start, const char *str)
{
	const char *p = str;

	while ( p > start && *(p-1) != '/' )
        --p;
    return p;
}
#endif

static const char *last_slash_or_end(const char *str)
{
	const char *p = str + strlen(str);

	while ( p > str && *(p-1) == '/' )
		p--;
	return p;
}

/**
   @this goes as far as it can in an iterative lookup. It always
   returns a node, and pointer to the token where lookup failed.

   If this returns an error, this is juste an informative error level
   about the failed lookup.

   @param root Process's root
   @param cwd Process's current working directory
   @param path Path to lookup
   @param end Where to end lookup. @tt end must be between @tt path
   and @tt{path + strlen(path)}
   @param next_place Next token to lookup (where lookup failed)
   @param node Node just before failed lookup

   @this transfers the ownership of @tt node to caller
 */
static
error_t vfs_lookup_part(
    struct vfs_node_s *root,
    struct vfs_node_s *cwd,
    const char *path,
    const char *end,
    const char **next_place,
    struct vfs_node_s **node)
{
	const char *token = path;
    error_t err = 0;

    vfs_printk("<%s '%s'->'%s' ", __FUNCTION__, path, end);

    /* Absolute lookup */
	if ( token[0] == '/' ) {
        vfs_printk("absolute ");
		token = next_nonslash(token);
		cwd = root;
	}

	*node = vfs_node_refnew(cwd);

    vfs_printk("start %p ", *node);

	while ( token < end ) {
        const char *slash_or_end = next_slash(token);
        err = 0;

        vfs_printk("part \"%s\"/%d ", token, slash_or_end-token);

        if ( ((slash_or_end-token) == 2)
			 && (token[0] == '.')
			 && (token[1] == '.') ) {
            if ( *node == root ) {
                // Trying to go above root, this is not permitted
                // Do nothing
            } else {
                struct vfs_node_s *parent = vfs_node_get_parent(*node);
                if ( ! parent )
                    break;
                vfs_node_refdrop(*node);
                *node = parent;
            }
            vfs_printk("parent->%p ", *node);
		} else if ( ((slash_or_end-token) == 1)
                    && (token[0] == '.') ) {
            // Self, do nothing
            vfs_printk("self->%p ", *node);
        } else {
            struct vfs_node_s *nnode;
            err = vfs_node_lookup(*node, token, slash_or_end-token, &nnode);
			if ( err ) {
                vfs_printk("other failed ");
                break;
            }
			vfs_node_refdrop(*node);
			*node = nnode;
            vfs_printk("other->%p ", *node);
		}

		token = next_nonslash(slash_or_end);
	}
    *next_place = token;
    vfs_printk("stopped at '%s'->'%s' n: %p : %d>", *next_place, end, *node, err);
	return err;
}

error_t vfs_lookup(struct vfs_node_s *root,
				   struct vfs_node_s *cwd,
				   const char *path,
				   struct vfs_node_s **node)
{
	if ( !path || !root || !cwd || !node )
		return -EINVAL;

	const char *end = last_slash_or_end(path);
	const char *where;

	error_t err = vfs_lookup_part(root, cwd, path, end, &where, node);
    if ( !err && (where >= end) )
        return 0;
    vfs_node_refdrop(*node);
    if ( err )
        return err;
    else
        return -ENOENT;
}


error_t vfs_create(struct vfs_node_s *root,
				   struct vfs_node_s *cwd,
				   const char *path,
				   enum vfs_node_type_e type,
				   struct vfs_node_s **node)
{
	if ( !path || !root || !cwd || !node )
		return -EINVAL;

	const char *end = last_slash_or_end(path);
	const char *prev = prev_slash(path, end);

	const char *last_part = next_nonslash(prev);

    /* We wont create "." or ".." */
    if ( last_part[0] == '.'
         && (((end-last_part) == 1)
             || ((last_part[1] == '.')
                 && ((end-last_part) == 2))
             ))
        return -EINVAL;

    struct vfs_node_s *parent;
	const char *stopped_at;

	error_t err = vfs_lookup_part(root, cwd, path, end, &stopped_at, &parent);
    if ( (err == -ENOENT) && (stopped_at >= last_part) )
        goto do_create;

    vfs_printk(" ! exists>\n");
    vfs_node_refdrop(parent);
    if ( err )
        return err;
    return -EEXISTS;

  do_create:

    {
        struct vfs_node_s *rnode;

        err = vfs_node_create(parent->fs, type, &rnode);
        vfs_printk("create %d %p ", err, rnode);
        if ( err ) {
            vfs_node_refdrop(parent);
            vfs_printk(" err>\n");
            return err;
        }

        err = vfs_node_link(rnode, parent, last_part, end-last_part, node);
        vfs_node_refdrop(parent);
        vfs_node_refdrop(rnode);
        vfs_printk("link %d %p>", err, *node);
        return err;
    }
}

error_t vfs_open(struct vfs_node_s *root,
				 struct vfs_node_s *cwd,
				 const char *path,
				 enum vfs_open_flags_e flags,
				 struct vfs_file_s **file)
{
	if ( !path || !root || !cwd || !file )
		return -EINVAL;

	const char *end = last_slash_or_end(path);
	const char *prev = prev_slash(path, end);

	const char *last_part = next_nonslash(prev);

    struct vfs_node_s *node;
	const char *stopped_at;

	error_t err = vfs_lookup_part(root, cwd, path, end, &stopped_at, &node);
    if ( (err == -ENOENT) && (stopped_at >= last_part) && (flags & VFS_OPEN_CREATE) )
        goto do_create;

    if ( err == 0 )
        goto do_open;

    vfs_printk(" parent %s>\n", strerror(err));
    vfs_node_refdrop(node);
    return err;

  do_create:

    vfs_printk("creating %s in %s", last_part, node->name);

    {
        // node is the parent directory

        struct vfs_node_s *created_node;
        struct vfs_node_s *linked_node;

        err = vfs_node_create(node->fs, VFS_NODE_FILE, &created_node);
        if ( err ) {
            vfs_printk(" create error>\n");
            vfs_node_refdrop(node);
            return err;
        }

        err = vfs_node_link(created_node, node, last_part, end-last_part, &linked_node);
        vfs_node_refdrop(node);
        vfs_node_refdrop(created_node);

        if ( err ) {
            vfs_printk(" link error>\n");
            return err;
        }

        node = linked_node;
    }
    
  do_open:
    vfs_printk("opening %s ", node->name);

    // node is node to open

	err = vfs_node_open(node, flags, file);
	vfs_node_refdrop(node);
	vfs_printk("%d %p>\n", err, *file);
	return err;
}

error_t vfs_stat(struct vfs_node_s *root,
				 struct vfs_node_s *cwd,
				 const char *path,
				 struct vfs_stat_s *stat)
{
	struct vfs_node_s *node;

	error_t err = vfs_lookup(root, cwd, path, &node);
	if ( err )
		return err;

	err = vfs_node_stat(node, stat);
	vfs_node_refdrop(node);
	return err;
}

error_t vfs_link(struct vfs_node_s *root,
                 struct vfs_node_s *cwd,
                 const char *src,
                 const char *dst)
{
	if ( !root || !cwd || !src || !dst )
		return -EINVAL;

	const char *dst_end = last_slash_or_end(dst);
	const char *dst_prev = prev_slash(dst, dst_end);

	const char *dst_last_part = next_nonslash(dst_prev);

    /* We wont create "." or ".." */
    if ( dst_last_part[0] == '.'
         && (((dst_end-dst_last_part) == 1)
             || ((dst_last_part[1] == '.')
                 && ((dst_end-dst_last_part) == 2))
             ))
        return -EINVAL;

    struct vfs_node_s *parent;
	const char *stopped_at;

	error_t err = vfs_lookup_part(root, cwd, dst, dst_end, &stopped_at, &parent);
    if ( (err == -ENOENT) && (stopped_at >= dst_last_part) )
        goto do_link;

    vfs_printk(" exists>\n");
    vfs_node_refdrop(parent);
    return err;

  do_link:

    {
        struct vfs_node_s *rnode;

        err = vfs_lookup(root, cwd, src, &rnode);
        vfs_printk("src lookup %d %p ", err, rnode);
        if ( err ) {
            vfs_node_refdrop(parent);
            vfs_printk(" err>\n");
            return err;
        }

        struct vfs_node_s *new_node;
        err = vfs_node_link(rnode, parent, dst_last_part, dst_end-dst_last_part, &new_node);
        if ( err == 0 )
            vfs_node_refdrop(new_node);
        vfs_node_refdrop(parent);
        vfs_node_refdrop(rnode);
        vfs_printk("link %d %p>", err, new_node);
        return err;
    }
}

error_t vfs_unlink(struct vfs_node_s *root,
				   struct vfs_node_s *cwd,
				   const char *path)
{
	struct vfs_node_s *node;

	error_t err = vfs_lookup(root, cwd, path, &node);
	if ( err )
		return err;

    // TODO also check we dont try to delete parent of any mountpoint
    // in the system.
	if ( node->fs->root == node->fs_node ) {
		vfs_node_refdrop(node);
		return -EBUSY;
	}

    struct vfs_node_s *parent = vfs_node_get_parent(node);
    if ( parent == NULL ) {
        vfs_node_refdrop(node);
        return -EUNKNOWN;
    }

	err = vfs_node_unlink(parent, node->name, strlen(node->name));
	vfs_node_refdrop(node);
    vfs_node_refdrop(parent);    
	return err;
}

static
void vfs_dump_item(struct vfs_node_s *node,
				   size_t pfx)
{
	size_t i;
	for (i=0; i<pfx; ++i)
		printk(" ");
    printk(" + %d \"%s\" %p (%p)"
#if defined(CONFIG_VFS_STATS)
           ", lu: %d, open: %d, close: %d, stat: %d"
#endif
//           ", free: %p"
           "\n"
           , vfs_node_refcount(node)
           , node->name
           , node, node->parent
#if defined(CONFIG_VFS_STATS)
           , atomic_get(&node->lookup_count)
           , atomic_get(&node->open_count)
           , atomic_get(&node->close_count)
           , atomic_get(&node->stat_count)
#endif
//           , node->obj_entry.storage_free
        );

    CONTAINER_FOREACH(vfs_dir_hash, HASHLIST, &node->children, {
            vfs_dump_item(item, pfx+2);
        });
}

void vfs_dump(struct vfs_node_s *root)
{
	printk("VFS dump for root %p, fsroot: %p, refcount: %d\n",
		   root, root->fs->root, atomic_get(&root->fs->ref));
	vfs_dump_item(root, 0);
}

void vfs_dump_lru(struct vfs_node_s *root)
{
	printk("VFS LRU dump for root %p, fsroot: %p\n",
		   root, root->fs->root);
    
    CONTAINER_FOREACH(vfs_lru, CLIST, &root->fs->lru_list, {
            vfs_dump_item(item, 2);
        });
}

void vfs_fs_dump_stats(struct vfs_fs_s *fs)
{
#if defined(CONFIG_VFS_STATS)
    printk(" node_open:    %d\n", atomic_get(&fs->node_open_count));
    printk(" lookup:       %d\n", atomic_get(&fs->lookup_count));
    printk(" create:       %d\n", atomic_get(&fs->create_count));
    printk(" link:         %d\n", atomic_get(&fs->link_count));
    printk(" unlink:       %d\n", atomic_get(&fs->unlink_count));
    printk(" stat:         %d\n", atomic_get(&fs->stat_count));
    printk(" node_create:  %d\n", atomic_get(&fs->node_create_count));
    printk(" node_destroy: %d\n", atomic_get(&fs->node_destroy_count));
    printk(" file_open:    %d\n", atomic_get(&fs->file_open_count));
    printk(" file_close:   %d\n", atomic_get(&fs->file_close_count));
#endif
}

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

