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

/**
   @file
   @module {Virtual File System}
   @short Operations on file handles

   @section {The node_open operation}
   @alias node_open
   
   When opening a node, @ref vfs_open_flags_e are passed to tell the
   VFS and the filesystem driver what type of operation is required.

   When opening directories, the only valid operation is to open
   read-only. Thus the correct flags are: @ref VFS_OPEN_READ @tt{|}
   @ref VFS_OPEN_DIR.

   When opening files, one may open with @ref VFS_OPEN_READ and/or
   @ref VFS_OPEN_WRITE. If @ref VFS_OPEN_APPEND is passed-in, @tt
   write operations will always write at the end of file.

   @ref VFS_OPEN_CREATE is only valid for creating files (so it is
   invalid with @ref VFS_OPEN_DIR), and is not supported at all layers
   of the VFS. It is valid when using @ref vfs_open, but invalid when
   using @tt node_open action of @ref vfs_fs_ops_s (because node
   already exists).
   
   @ref VFS_OPEN_TRUNCATE is to erase contents of a file at opening,
   filesystem is responsible for implementing this flag.
   
   @end section

 */

#ifndef _VFS_FILE_H_
#define _VFS_FILE_H_

#include <hexo/decls.h>

C_HEADER_BEGIN

#include <hexo/types.h>
#include <gpct/object_refcount.h>
#include <vfs/defs.h>

enum vfs_open_flags_e
{
    /** Allow read operation */
    VFS_OPEN_READ = 1,
    /** Allow write operation */
    VFS_OPEN_WRITE = 2,
    /** Create the file if nonexistant */
    VFS_OPEN_CREATE = 4,
    /** Always write at end */
    VFS_OPEN_APPEND = 8,
    /** Erase contents of file on open, this is filesystem-implemented */
    VFS_OPEN_TRUNCATE = 16,
    /** Open a directory (only valid with VFS_OPEN_READ) */
    VFS_OPEN_DIR = 32,
};

enum vfs_whence_e
{
    /** Seek from start of file */
	VFS_SEEK_SET,
    /** Seek from end of file */
	VFS_SEEK_END,
    /** Seek from current point in file */
	VFS_SEEK_CUR,
};

struct vfs_node_s;
struct vfs_file_s;

/** @this defines the file close operation prototype */
#define VFS_FILE_CLOSE(x) error_t (x)(struct vfs_file_s *file)

/**
   @this closes an open file descriptor.  The file descriptor must not
   be used afterwards.

   @param file File descriptor to close
   @return 0 on successful close

   @csee #VFS_FILE_CLOSE
 */
typedef VFS_FILE_CLOSE(vfs_file_close_t);


/** @this defines the file read operation prototype */
#define VFS_FILE_READ(x) ssize_t (x)(struct vfs_file_s *file, void *buffer, size_t size)

/**
   @this reads from a file.

   @param file File descriptor to read from
   @param buffer User buffer to fill
   @param size Size of transfer in bytes
   @return @tt size or less for valid transfers, 0 on end-of-file
   condition, a negative number on error conditions

   @csee #VFS_FILE_READ
 */
typedef VFS_FILE_READ(vfs_file_read_t);


/** @this defines the file write operation prototype */
#define VFS_FILE_WRITE(x) ssize_t (x)(struct vfs_file_s *file, const void *buffer, size_t size)

/**
   @this writes to a file.

   @param file File descriptor to write to
   @param buffer User buffer to read data from
   @param size Size of transfer in bytes
   @return @tt size or less for valid transfers, 0 on end-of-file
   condition, a negative number on error conditions

   @csee #VFS_FILE_WRITE
 */
typedef VFS_FILE_WRITE(vfs_file_write_t);



/** @this defines the file seek operation prototype */
#define VFS_FILE_SEEK(x) off_t (x)(struct vfs_file_s *file, off_t offset, enum vfs_whence_e whence)

/**
   @this changes current point into a file.

   @param file File descriptor to seek in
   @param offset Offset to move the point by
   @param whence Reference point to calculate the offset from
   @return the new absolute point from start of file

   @this function may be used to seek beyond end of file. This is not
   an error.

   @csee #VFS_FILE_SEEK 
 */
typedef VFS_FILE_SEEK(vfs_file_seek_t);


/** @this defines the file truncate operation prototype */
#define VFS_FILE_TRUNCATE(x) off_t (x)(struct vfs_file_s *file, off_t new_size)

/**
   @this changes size of a file to exactly @tt new_size

   @param file File to truncate
   @param new_size Offset to cut at
   @return 0 if done

   @this function may be used to truncate beyond end of file. This
   will extent the file with zeros.

   @csee #VFS_FILE_TRUNCATE 
 */
typedef VFS_FILE_TRUNCATE(vfs_file_truncate_t);


OBJECT_TYPE     (vfs_file, REFCOUNT, struct vfs_file_s);
OBJECT_PROTOTYPE(vfs_file, static inline, vfs_file);

struct vfs_file_s
{
	vfs_file_entry_t obj_entry;         
	struct fs_node_s *node;            //< Corresponding node in the FS
	vfs_fs_node_refdrop_t *node_refdrop; //< Function to call on close
	vfs_file_close_t *close;            //< Close operation for this file  
	vfs_file_read_t *read;              //< Read operation for this file   
	vfs_file_write_t *write;            //< Write operation for this file  
	vfs_file_seek_t *seek;              //< Seek operation for this file   
	vfs_file_truncate_t *truncate;      //< Truncate operation for this file   
	off_t offset;                       //< Current access position in file
	void *priv;                         //< File system private data
};

struct vfs_dirent_s
{
    /** Name of the directory entry, asciiZ */
	char name[CONFIG_VFS_NAMELEN + 1];
    /** Type of node */
	enum vfs_node_type_e type;
    /** Size of file in bytes, or count of children nodes excluding
        "." and ".." */
	size_t size;
};

OBJECT_CONSTRUCTOR(vfs_file);
OBJECT_DESTRUCTOR(vfs_file);

OBJECT_FUNC   (vfs_file, REFCOUNT, static inline, vfs_file, obj_entry);

#ifdef __MKDOC__
/**
   @this creates a new opened file object. All file access operations are
   initialized with default error returning functions. The close operation
   defaults to a @ref vfs_file_refdrop call.

   @param storage pointer to pre-allocated memory for new vfs node, may be NULL.
   @param node associated fs node
   @param node_refnew fs node refnew func
   @param node_refdrop fs node refdrop func
   @return the new file object.
 */
struct file_s * vfs_file_new(void *storage, struct vfs_node_s * node, vfs_fs_node_refnew_t *node_refnew, vfs_fs_node_refdrop_t *node_refdrop);
#endif

/** @this increases the file reference count and return the file itself. */
struct vfs_file_s * vfs_file_refnew(struct vfs_file_s * file);

/** @this decreases the file reference count and may delete the file if no more reference exist. */
void vfs_file_refdrop(struct vfs_file_s * file);

/**
   @this reads from an opened file

   @param file File descriptor
   @param buffer Buffer to read into
   @param size Size of the transfer
   @return the size of buffer actually read
*/
static inline ssize_t vfs_file_read(struct vfs_file_s *file,
					  void *buffer,
					  size_t size)
{
	return file->read(file, buffer, size);
}

/**
   @this writes from an opened file

   @param file File descriptor
   @param buffer Buffer to write into
   @param size Size of the transfer
   @return the size of buffer actually written
*/
static inline ssize_t vfs_file_write(struct vfs_file_s *file,
					   const void *buffer,
					   size_t size)
{
	return file->write(file, buffer, size);
}

/**
   @this closes an opened file

   @param file File descriptor to close
   @return 0 if closed correctly
*/
static inline error_t vfs_file_close(struct vfs_file_s *file)
{
	return file->close(file);
}

/**
   @this seeks to the given position into an opened file

   @param file File descriptor to seek into
   @param offset To seek from given point
   @param whence Reference point to seek from
   @return new absolute position from the beginning of file
*/
static inline off_t vfs_file_seek(struct vfs_file_s *file,
					  off_t offset,
					  enum vfs_whence_e whence)
{
	return file->seek(file, offset, whence);
}

/**
   @this truncates the file to the exact size @tt new_size

   @param file File to truncate
   @param new_size New file size
   @return 0 if done
*/
static inline off_t vfs_file_truncate(struct vfs_file_s *file,
					  off_t new_size)
{
	return file->truncate(file, new_size);
}

/** @this provides a file operations function set */
extern const struct fileops_s vfs_file_fops;

C_HEADER_END

#endif

// Local Variables:
// tab-width: 4
// c-basic-offset: 4
// c-file-offsets:((innamespace . 0)(inline-open . 0))
// indent-tabs-mode: nil
// End:

// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=4:softtabstop=4

