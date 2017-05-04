/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/

/**
 * @file
 * @module{Device drivers}
 * @short Devices definitions
 */

#ifndef __DEVICE_H__
#define __DEVICE_H__

struct device_s;
struct driver_s;

#include <hexo/types.h>
#include <hexo/error.h>

enum device_class_e
  {
    device_class_none = 0,

    device_class_block,
    device_class_char,
    device_class_enum,
    device_class_fb,
    device_class_icu,
    device_class_input,
    device_class_net,
    device_class_sound,
    device_class_timer,
    device_class_spi,
    device_class_lcd,
    device_class_gpio,
    device_class_i2c,
    device_class_mem,
  };


/** Common class irq() function template. */
#define DEV_IRQ(n)	bool_t (n) (struct device_s *dev)

/** Common device class irq() function type. Must be called on
    interrupt request.

    * @param dev pointer to device descriptor
    * @return 1 if interrupt have been handled by the device
    */
typedef DEV_IRQ(dev_irq_t);




/** Common class create() function template. */
#define DEV_CREATE(n)	error_t (n) (struct device_s *parent, void *params)

/** Common device class create() methode shortcut */
#define dev_create(param) (dev)->drv->f_create(param)

/** Common device class create() function. This function must be used
    to create new virtual devices if driver support this.

    * @param parent parent device for new virtual devices if any.
    * @param params driver dependent parameters, NULL if none
    * @return negative error code, or number of created devices on success.
    */
typedef DEV_CREATE(dev_create_t);




/** Common class init() function template. */
#define DEV_INIT(n)	error_t (n) (struct device_s *dev, void *params)

/** Common device class init() methode shortcut */
#define dev_init(dev, ...) (dev)->drv->f_init(dev, __VA_ARGS__)

/** Common device class init() function type. This function will init
    the hardware device and must be called before using any other
    functions on the device. This function will allocate device
    private data.

    * @param dev pointer to device descriptor
    * @param icudev pointer to associated interrupt controller device
    * @param params driver dependent parameters, NULL if none
    * @return negative error code, 0 on succes
    */
typedef DEV_INIT(dev_init_t);




/** Common device class cleanup() function template. */
#define DEV_CLEANUP(n)	void    (n) (struct device_s *dev)

/** Common device class cleanup() methode shortcut */
#define dev_cleanup(dev) (dev)->drv->f_cleanup(dev)

/** Common device class cleanup() function type. Free all ressources
    allocated with the init() function.

   * @param dev pointer to device descriptor
   */
typedef DEV_CLEANUP(dev_cleanup_t);




/** Device descriptor structure */

#define DEVICE_MAX_ADDRSLOT	4

#include <hexo/gpct_platform_hexo.h>
#include <hexo/gpct_lock_hexo.h>

#ifdef CONFIG_DEVICE_TREE

#include <gpct/cont_clist.h>
#include <gpct/object_refcount.h>

OBJECT_TYPE(device_obj, REFCOUNT, struct device_s);

OBJECT_PROTOTYPE(device_obj, static inline, device_obj);

#endif

/** device object structure */

#ifdef CONFIG_DEVICE_TREE

#define CONTAINER_LOCK_device_list HEXO_SPIN
#define CONTAINER_OBJ_device_list device_obj

CONTAINER_TYPE(device_list, CLIST,
#endif
struct device_s
{
  const struct driver_s		*drv;
  struct device_s		*icudev;

  /** general purpose device lock */
  lock_t			lock;

  /** pointer to device driver private data if any */
  void				*drv_pv;

  /** hardware interrupt line number */
  int_fast8_t			irq;

  /** device IO addresses table */
  uintptr_t			addr[DEVICE_MAX_ADDRSLOT];

#ifdef CONFIG_DEVICE_TREE
  /** pointer to device enumrator private data if any */
  void				*enum_pv;

  struct device_s		*parent;
  device_list_entry_t		list_entry;
  device_obj_entry_t		obj_entry;
  device_list_root_t		children;
#endif /* !CONFIG_DEVICE_TREE */

}
#ifdef CONFIG_DEVICE_TREE
, list_entry)
#endif
;

/* used when no irq line is present/available */
#define DEVICE_IRQ_INVALID	-1

#ifdef CONFIG_DEVICE_TREE

OBJECT_CONSTRUCTOR(device_obj);
OBJECT_DESTRUCTOR(device_obj);

OBJECT_FUNC(device_obj, REFCOUNT, static inline, device_obj, obj_entry);

CONTAINER_PROTOTYPE(device_list, inline, device_list);

error_t device_register(struct device_s *dev,
			struct device_s *parent,
			void *enum_pv);

error_t device_unregister(struct device_s *dev);

void device_dump_list(struct device_s *root);

#define DEVICE_TREE_WALKER(x) void (x)(struct device_s *dev, void *priv)

typedef DEVICE_TREE_WALKER(device_tree_walker_t);

void device_tree_walk(device_tree_walker_t *walker, void *priv);

#endif /* !CONFIG_DEVICE_TREE */

void device_init(struct device_s *dev);
struct device_s *device_get_child(struct device_s *dev, uint_fast8_t i);

#ifdef CONFIG_VMEM
uintptr_t vpage_io_map(paddr_t paddr, size_t size);
#endif

static inline
error_t device_mem_map(struct device_s *dev, uint_fast8_t mask)
{
#if defined( CONFIG_VMEM )
  uint_fast8_t i;
  for( i = 0 ; i < ( sizeof(uint_fast8_t) * 8 ) ; i++ )
    {
    if( mask & 0x1 )
      dev->addr[ i ] = vpage_io_map( dev->addr[ i ], 1 );
    mask >>= 1;
    }
#endif
  return 0;
}

#ifdef CONFIG_DEVICE_TREE
#define DEVICE_INITIALIZER							\
{										\
  .children = CONTAINER_ROOT_INITIALIZER(device_list, CLIST),			\
  .obj_entry = OBJECT_INITIALIZER(device_obj, REFCOUNT)				\
}
#else
#define DEVICE_INITIALIZER	{ }
#endif

#endif

