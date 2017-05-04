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

#ifndef FATFS_TYPES_H
#define FATFS_TYPES_H

struct fat_dirent_s
{
	uint8_t name[11];
	uint8_t attr;
	uint8_t ntres;
	uint8_t create_time_tenth;
	uint16_t create_time;
	uint16_t create_date;
	uint16_t access_date;
	uint16_t clust_hi;
	uint16_t update_time;
	uint16_t update_date;
	uint16_t clust_lo;
	uint32_t file_size;
};

#define FATFS_DIRENT_FREE 0xa5
#define FATFS_DIRENT_A5 0x05
#define FATFS_DIRENT_LAST 0x00

#endif
