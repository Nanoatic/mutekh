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

  Copyright Alexandre Becoulet, <alexandre.becoulet@free.fr>, 2009
*/

#ifndef _ISO9660_BITS_H_
#define _ISO9660_BITS_H_

#define ISO9660_BLOCK_SIZE	2048

/* Iso9660 has some fields in both little and big endian versions */
#ifdef CONFIG_CPU_ENDIAN_BIG
# define ISO9660_ENDIAN(type, name) type name##_le; type name;
#else
# define ISO9660_ENDIAN(type, name) type name; type name##_be;
#endif

/* ISO9660 directory structure */
#define ISO9660_DATE_LEN	7

enum			iso9660_file_type_e
  {
    iso9660_file_hidden	= 1,	/* File is Hidden if this bit is 1 */
    iso9660_file_isdir	= 2,	/* Entry is a Directory if this bit is 1 */
    iso9660_file_associat = 4,	/* Entry is an Associated file is this bit is 1 */
    iso9660_file_useext	= 8,	/* Information is structured according to the extended attribute record if this bit is 1 */
    iso9660_file_useperm = 16,	/* Permissions are specified in the extended attribute record if this bit is 1 */
    iso9660_file_multidir = 128	/* File has more than one directory record if this bit is 1 */
  };

struct			iso9660_dir_s
{
  uint8_t		dir_size;	/* Length of directory record */
  uint8_t		ext_size;	/* Extended attribute record length */
  ISO9660_ENDIAN(uint32_t, data_blk);	/* File data block index */
  ISO9660_ENDIAN(uint32_t, file_size);	/* File size */
  char			date[ISO9660_DATE_LEN];
  uint8_t		type;		/* File type (enum iso_file_type_e) */

  /* only valid if the file is recorded in interleave mode */
  uint8_t		unit_size;	/* File Unit Size */
  uint8_t		gap_size;	/* Interleave Gap Size */

  ISO9660_ENDIAN(uint16_t, vol_seq);
  uint8_t		idf_len;
  char			idf[0];		/* file name */
}			__attribute__ ((packed));

/* ISO9660 Primary volume descriptor structure */

#define ISO9660_PRIM_VOLDESC_BLOCK	16

#define ISO9660_SYSIDF_LEN	32
#define ISO9660_VOLIDF_LEN	32
#define ISO9660_VOLSET_LEN	128
#define ISO9660_PUBIDF_LEN	128
#define ISO9660_DPREP_LEN	128
#define ISO9660_APP_LEN		128
#define ISO9660_CPRFIL_LEN	37
#define ISO9660_ABSFIL_LEN	37
#define ISO9660_BIBFIL_LEN	37
#define ISO9660_LDATE_LEN	17

struct				iso9660_prim_voldesc_s
{				
  uint8_t			vol_desc_type;		/* Volume Descriptor Type (1) */
  char				std_identifier[5];	/* Standard Identifier (CD001) */
  uint8_t			vol_desc_version;	/* Volume Descriptor Version (1) */
  uint8_t			unused1;		/* Unused Field */
  char				sys_idf[ISO9660_SYSIDF_LEN]; /* System Identifier */
  char				vol_idf[ISO9660_VOLIDF_LEN]; /* Volume Identifier */
  uint8_t			unused2[8];		/* Unused Field */
  ISO9660_ENDIAN(uint32_t,	vol_blk_count);	/* Number of logical blocks in the Volume */
  uint8_t			unused4[32];		/* Unused Field */
  ISO9660_ENDIAN(uint16_t,	vol_set_size);	/* The Volume Set size of the Volume */
  ISO9660_ENDIAN(uint16_t,	vol_seq_num);	/* The number of the volume in the Set */
  ISO9660_ENDIAN(uint16_t,	vol_blk_size);	/* The size in bytes of a Logical Block */
  ISO9660_ENDIAN(uint32_t,	path_table_size);	/* Length in bytes of the path table */
  ISO9660_ENDIAN(struct {
    uint32_t			path_table_blk;		/* path table block index */
    uint32_t			opath_table_blk;	/* optional path table block index */
  },                    	table_blk);
				
  struct iso9660_dir_s		root_dir;		/* Root directory entry */
  uint8_t			unused5[34 - sizeof (struct iso9660_dir_s)];/* padding */
				
  char				volset_idf[ISO9660_VOLSET_LEN];	/* Name of the multiple volume set */
  char				pub_idf[ISO9660_PUBIDF_LEN];	/* Publisher name */
  char				dprep_idf[ISO9660_DPREP_LEN];	/* Data preparer name */
  char				app_idf[ISO9660_APP_LEN];	/* Application name */
				
  char				copyright_file[ISO9660_CPRFIL_LEN]; /* Copyright file name in root dir */
  char				abstract_file[ISO9660_ABSFIL_LEN]; /* Abstract file name in root dir */
  char				bibli_file[ISO9660_BIBFIL_LEN]; /* Bibliographic file name in root dir */
  char				date_creat[ISO9660_LDATE_LEN]; /* Creation date */
  char				date_modif[ISO9660_LDATE_LEN]; /* Modification date */
  char				date_expir[ISO9660_LDATE_LEN]; /* Expiration date */
  char				date_effect[ISO9660_LDATE_LEN]; /* Effective date */
  uint8_t			filestrutc_version;	/* File Structure Version (1) */
} __attribute__ ((packed));

/* System Use Sharing Protocol for Rock Ridge dir entries */

struct				iso9660_susp_s
{
  uint16_t			sign;
  uint8_t			len;
  uint8_t			version;
  uint8_t			data[0];
} __attribute__ ((packed));

struct				iso9660_susp_nm_s
{
  struct iso9660_susp_s		hdr;
  uint8_t			flag;
  char				name[0];
} __attribute__ ((packed));

struct				iso9660_susp_ce_s
{
  struct iso9660_susp_s		hdr;
  ISO9660_ENDIAN(uint32_t,	block);
  ISO9660_ENDIAN(uint32_t,	offset);
  ISO9660_ENDIAN(uint32_t,	size);
} __attribute__ ((packed));

#endif

