
#ifndef _SYS_STAT_H_
#define _SYS_STAT_H_

/**
 * @file
 * @module{C library}
 */

struct stat
{
#if 0
  dev_t     st_dev;     /* ID of device containing file */
  ino_t     st_ino;     /* inode number */
#endif
  mode_t    st_mode;    /* protection */
#if 0
  nlink_t   st_nlink;   /* number of hard links */
  uid_t     st_uid;     /* user ID of owner */
  gid_t     st_gid;     /* group ID of owner */
  dev_t     st_rdev;    /* device ID (if special file) */
#endif
  off_t     st_size;    /* total size, in bytes */
#if 0
  blksize_t st_blksize; /* blocksize for file system I/O */
  blkcnt_t  st_blocks;  /* number of blocks allocated */
  time_t    st_atime;   /* time of last access */
  time_t    st_mtime;   /* time of last modification */
  time_t    st_ctime;   /* time of last status change */
#endif
};

#define __S_IFMT        0170000 /* file type.  */

#define S_IFDIR 0040000 /* Directory.  */
#define S_IFCHR 0020000 /* Character device.  */
#define S_IFBLK 0060000 /* Block device.  */
#define S_IFREG 0100000 /* Regular file.  */
#define S_IFIFO 0010000 /* FIFO.  */
#define S_IFLNK 0120000 /* Symbolic link.  */
#define S_IFSOCK        0140000 /* Socket.  */

#define S_ISUID 04000   /* Set user ID on execution.  */
#define S_ISGID 02000   /* Set group ID on execution.  */
#define S_ISVTX 01000   /* Save swapped text after use (sticky).  */

#define S_IRUSR 0400    /* Read by owner.  */
#define S_IWUSR 0200    /* Write by owner.  */
#define S_IXUSR 0100    /* Execute by owner.  */
#define S_IRWXU (S_IRUSR | S_IWUSR | S_IXUSR)

#define S_IRGRP (S_IRUSR >> 3)  /* Read by group.  */
#define S_IWGRP (S_IWUSR >> 3)  /* Write by group.  */
#define S_IXGRP (S_IXUSR >> 3)  /* Execute by group.  */
#define S_IRWXG (S_IRWXU >> 3)

#define S_IROTH (S_IRGRP >> 3)  /* Read by others.  */
#define S_IWOTH (S_IWGRP >> 3)  /* Write by others.  */
#define S_IXOTH (S_IXGRP >> 3)  /* Execute by others.  */
#define S_IRWXO (S_IRWXG >> 3)

#define __S_ISTYPE(mode, mask)  (((mode) & __S_IFMT) == (mask))

#define S_ISDIR(mode)    __S_ISTYPE((mode), S_IFDIR)
#define S_ISREG(mode)    __S_ISTYPE((mode), S_IFREG)

#endif

