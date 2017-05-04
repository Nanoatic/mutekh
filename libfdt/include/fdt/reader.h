#ifndef FDT_READ_H_
#define FDT_READ_H_

/**
   @file
   @module{FDT access library}
   @short Read-only access to FDT blobs
 */

#include <hexo/types.h>
#include <hexo/error.h>
#include <hexo/endian.h>

struct fdt_walker_s;
struct fdt_walker_state_s;

/**
   on_node_entry prototype macro
   @see fdt_on_node_entry_func_t
 */
#define FDT_ON_NODE_ENTRY_FUNC(x) bool_t (x)(						   \
		void *priv,                                                    \
		struct fdt_walker_state_s *state,							   \
		const char *path)

/**
   on_node_leave prototype macro
   @see fdt_on_node_leave_func_t
 */
#define FDT_ON_NODE_LEAVE_FUNC(x) void (x)(void *priv)

/**
   on_node_prop prototype macro
   @see fdt_on_node_prop_func_t
 */
#define FDT_ON_NODE_PROP_FUNC(x) void (x)(							   \
		void *priv,                                                    \
		struct fdt_walker_state_s *state,							   \
		const char *name,											   \
		const void *data,											   \
		size_t datalen)

/**
   on_mem_reserve prototype macro
   @see fdt_on_mem_reserve_func_t
 */
#define FDT_ON_MEM_RESERVE_FUNC(x) void (x)(						   \
		void *priv,                                                    \
		uint64_t addr,												   \
		uint64_t size)

/**
   Type definition for entry in a new node. As nodes may be nested,
   this function may be called many times in a row.

   @param priv private data provided in the @ref fdt_walker_s
   @param offset node offset from the beginning of the structure, this
          can be used to resolve references
   @param path full path of the node
   @return whether user is interested in this node, its properties and
           its subnodes.
   @see #FDT_ON_NODE_ENTRY_FUNC
 */
typedef FDT_ON_NODE_ENTRY_FUNC(fdt_on_node_entry_func_t);

/**
   Type definition for end of a node. As nodes may be nested, this
   function may be called many times in a row.

   @param priv private data provided in the @ref fdt_walker_s
   @see #FDT_ON_NODE_LEAVE_FUNC
 */
typedef FDT_ON_NODE_LEAVE_FUNC(fdt_on_node_leave_func_t);

/**
   Type definition for function called on a node property. As
   properties are inside nodes, on_node_entry has already be called
   once when calling this function.

   @param priv private data provided in the @ref fdt_walker_s
   @param offset offset of the parameter in the structure
   @param name name of the parameter
   @param data pointer to raw data. User must take care of the meaning
          by itself. If data contains numeric values, they are
          stored bigendian.
   @param datalen length of the data, in bytes
   @see #FDT_ON_NODE_PROP_FUNC
 */
typedef FDT_ON_NODE_PROP_FUNC(fdt_on_node_prop_func_t);

/**
   Type definition for function called on a memory reservation
   node. Note values are always 64 bits for this type of nodes.

   There is no endian adaptation to perform on the parameters.

   @param priv private data provided in the @ref fdt_walker_s
   @param addr base address of reservation
   @param size size of reservation
   @see #FDT_ON_MEM_RESERVE_FUNC
 */
typedef FDT_ON_MEM_RESERVE_FUNC(fdt_on_mem_reserve_func_t);

/**
   Structure containing pointers to user-provided functions and
   private data. When using @ref fdt_walk_blob with this structure,
   all function pointers must be provided. Leaving a NULL pointer to
   function will lead to unpredictable results.
 */
struct fdt_walker_s
{
  void *priv; //< User-owned pointer, ignored by walker
  fdt_on_node_entry_func_t *on_node_entry; //< Function to call entering a node
  fdt_on_node_leave_func_t *on_node_leave; //< Function to call leaving a node
  fdt_on_node_prop_func_t *on_node_prop; //< Function to call for each property
  fdt_on_mem_reserve_func_t *on_mem_reserve; //< Function to call for each memory reservation map
};

/**
   @this processes a whole blob calling the provided functions when
   needed. The blob contains its size in its own header.

   @param blob Pointer to the FDT blob header
   @param walker User-provided functions
   @return whether the parsing went well, to the end
 */
error_t fdt_walk_blob(const void *blob, struct fdt_walker_s *walker);

/**
   @this retrieves the structure offset for the current state.

   This is not valid when walking through memory reservation.

   @param state Internal state of the parser
   @return the current token offset
 */
uint32_t fdt_reader_get_struct_offset(struct fdt_walker_state_s *state);

/**
   @this retrieves the value of a property inside a node, if it
   exists.  Calling this function is only valid when inside a
   on_node_entry.

   @param state Internal state of the parser
   @param prop Property name to look for
   @param propval return pointer to the property value, if found
   @param propsize return size of property value, if found
   @return 1 if the property has been found, 0 otherwise
 */
bool_t fdt_reader_has_prop(const struct fdt_walker_state_s *state,
						   const char *propname,
						   const void **propval, size_t *propsize);

/**
   @this gets the complete size of a blob as told in its header.

   @param blob a pointer to the start of a device tree blob
   @return the total size of the blob, 0 if the blob is invalid
 */
size_t fdt_get_size(void *blob);

/**
   @this processes a blob calling the provided functions when needed,
   only from the given offset in the blob. The offset must be a value
   previously returned by @ref fdt_reader_get_struct_offset, and has
   no meaningful value outside this context.  This function will never
   walk the memory reservation nodes. The corresponding pointer in
   walker may be @ref NULL.

   @param blob Pointer to the FDT blob header
   @param walker User-provided functions
   @param offset The subnode offset to walk
   @return whether the parsing went well, to the end
 */
error_t fdt_walk_blob_from(const void *blob,
						   struct fdt_walker_s *walker,
						   uint32_t offset);

/**
   @this retrieves a particular memory reservation entry from the
   table.  This function does not check for bounds.  User can tell it
   reached the last entry when *addr and *size are 0.

   @param blob Pointer to the FDT blob header
   @param resno Index of the memory reservation in the map, 0-indexed
   @param addr Pointer @this must fill with the address of the
   reservation
   @param size Pointer @this must fill with the size of the
   reservation
 */
void fdt_get_rsvmap(const void *blob, uint32_t resno,
					uint64_t *addr, uint64_t *size);


/**
   @this parses an integer value from the blob. It abides #*-cell for
   the input data, and a size of output data.

   @param cells Number of cells for one integer starting at data
   @param data Pointer to a data cell
   @param retval_size User-provided sizeof(*retval)
   @param retval Pointer to a data value to fill
   @return the memory following the parsed data cell
 */
static inline void* fdt_parse_sized( uint8_t cells, const void *data,
									 uint8_t retval_size, void *retval )
{
	if ( retval && retval_size ) {
		switch (cells) {
		case 1:
			switch (retval_size) {
			case 4:
				*(uint32_t*)retval = endian_be32(*(uint32_t*)data);
				break;
			case 8:
				*(uint64_t*)retval = endian_be32(*(uint32_t*)data);
				break;
			}
			break;
		case 2:
			switch (retval_size) {
			case 4:
				*(uint32_t*)retval = endian_be64(*(uint64_t*)data);
				break;
			case 8:
				*(uint64_t*)retval = endian_be64(*(uint64_t*)data);
				break;
			}
			break;
		}
	}

	return (void*)((uintptr_t)data + cells * 4);
}


/**
   @this retrieves an integer value from the current node. This is a
   shortcut for @ref fdt_reader_has_prop followed by @ref
   fdt_parse_sized.

   @param state Walker state
   @param propname Property name to lookup
   @param rval Pointer to an integer to fill
   @param rsize Size of the data pointed by rval, in bytes
   @return whether a value has been found
 */
static inline
bool_t fdt_reader_get_prop_int(const struct fdt_walker_state_s *state,
							   const char *propname,
							   void *rval, size_t rsize)
{
	const void *binval;
	size_t binsize;

	if ( fdt_reader_has_prop(state, propname, &binval, &binsize) ) {
		fdt_parse_sized(1, binval, rsize, rval);
		return 1;
	}
	return 0;
}

#endif
