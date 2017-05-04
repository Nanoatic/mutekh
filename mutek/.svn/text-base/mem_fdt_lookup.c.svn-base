#include <hexo/segment.h>
#include <hexo/types.h>
#include <mutek/printk.h>
#include <fdt/reader.h>

#include <mutek/mem_alloc.h>
#include <mutek/mem_region.h>
#include <mutek/memory_allocator.h>


struct lookup_state_s
{
  uint8_t addr_cells;
  uint8_t size_cells;

  uintptr_t addr;
  size_t size;
  bool_t cached;
};

static void parse_reg_size( struct lookup_state_s *pv, const void *data )
{
	uint_fast8_t i;
	const void *ptr = data;

	ptr = fdt_parse_sized( pv->addr_cells, ptr,
			       sizeof(pv->addr), &pv->addr );
	ptr = fdt_parse_sized( pv->size_cells, ptr,
			       sizeof(pv->size), &pv->size );
}


static bool_t sys_in_segment( struct lookup_state_s *priv )
{
  if ( priv->addr <= CONFIG_RAM_ADDR &&
       priv->addr + priv->size > CONFIG_RAM_ADDR)
    return true;
  return false;
}

//TODO: Add memory reservation support and default_region extend
static FDT_ON_NODE_ENTRY_FUNC(mem_lookup_node_entry)
{
	struct lookup_state_s *private = priv;

	const void *devtype = NULL;
	size_t devtypelen;
	const void *reg = NULL;
	size_t reglen;
	
	uint_fast16_t i;
	uint64_t addr;
	uint64_t size;
	
	if ( fdt_reader_has_prop(state, "device_type", &devtype, &devtypelen ) ) {
		if ( !strcmp( devtype, "memory" ) ) {
		  /* Memory segment found*/
		  /* 1: get info: cacheability, addr and size*/
		  private->cached = 0;
		  if ( fdt_reader_has_prop(state, "cached", NULL, NULL ) )
		    private->cached = 1;

		  if ( fdt_reader_has_prop(state, "reg", &value, &len ) )
		    {
		      parse_reg_size( private, value);
		    }
		  else
		    return 0;
		  
		  /* 2: verify if no reservations are done for this segment*/
		  
		  /* 3: verify if the system scope is allocated in this segment. Then extend the system scope if possible */
		  if ( sys_in_segment(pv) )
		    {
		      //extend_sys_scope(pv);
		      return 0;
		    }
		  
		  /* 4: create a region associated to this segment*/
		  struct memory_allocator_region_s *region;
		  
		  if (private->cached)
		    region = memory_allocator_init(mem_region_get_first(mem_scope_sys)->region,
					  mem_info.base,
					  mem_info.base + mem_info.size);
		  else
		    region = memory_allocator_init(NULL,
					  mem_info.base,
					  mem_info.base + mem_info.size);
		  
		  mem_region_add(mem_scope_sys, region, 100);
		  
		  /* 5: make reservation inside the region corresponding to reserve map in fdt*/

		  /* Next step: looking in the topology, and add segment to the corresponding list*/
		}
	}
}

static FDT_ON_NODE_ENTRY_FUNC(topology_lookup_node_entry)
{
  struct lookup_state_s *private = priv;
  
  switch (private->state) 
    {
    case IN_ROOT:
      if ( !strcmp(path, "/topology") )
	{
	  private->state = IN_TOPOLOGY;
	}
      break;
    case IN_TOPOLOGY:
      private->state = IN_CLUSTER;
      
      break;
    default:
      break;
    }
  
}

static FDT_ON_NODE_PROP_FUNC(cell_lookup_node_prop)
{
  struct lookup_state_s *private = priv;
  
  if ( !strcmp( name, "#address-cells" ) )
    private->addr_cells = endian_be32(*(uint32_t*)data);
  else if ( !strcmp( name, "#size-cells" ) )
    private->size_cells = endian_be32(*(uint32_t*)data);
}


static FDT_ON_NODE_LEAVE_FUNC(topology_node_leave)
{
  switch (private->state) 
    {
    case IN_ROOT:
      break;
    case IN_TOPOLOGY:
      private->state = IN_ROOT;
      break;
    case IN_CLUSTER:
      private->state = IN_TOPOLOGY;
      break;
    default:
      break;
    }
}


static FDT_ON_NODE_LEAVE_FUNC(nop_node_leave)
{
}


static FDT_ON_MEM_RESERVE_FUNC(nop_reserve)
{
}


void mem_lookup_parse_fdt(void *blob,  )
{
  struct lookup_state_s mem_priv;
  
  struct fdt_walker_s walker = {
    .priv = &mem_priv,
    .on_node_entry = mem_lookup_node_entry,
    .on_node_leave = nop_node_leave,
    .on_node_prop = cell_lookup_node_prop,
    .on_mem_reserve = nop_mem_reserve,
  };
  
  fdt_walk_blob(blob, &walker);

  struct topology_lookup_state_s top_priv;

  walker.priv = &top_priv,
  walker.on_node_entry = topology_lookup_node_entry,

  fdt_walk_blob(blob, &walker);
}
