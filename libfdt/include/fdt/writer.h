#ifndef FDT_WRITER_H_
#define FDT_WRITER_H_

/**
   @file
   @module{FDT access library}
   @short Write-only access to FDT blobs
 */

#include <hexo/types.h>
#include <hexo/error.h>

/**
 * An opaque structure containing the internal state of the
 * writer.
 */
struct fdt_writer_s
#ifndef FDT_INTERNAL
{
	void *opaque[7];
}
#endif
;

/**
   @this initializes the writer internal state, give a memory region
   to the writer code. If this memory region is not big enough, caller
   wont be notified until @ref fdt_writer_finalize is called.

   @param writer A writer opaque structure to initialize
   @param blob A buffer where the writer will put the blob
   @param available_size buffer size, in bytes
   @return whether it went well
 */
error_t fdt_writer_init(
	struct fdt_writer_s *writer,
	void *blob,
	size_t available_size);

/**
   @this adds a reservation entry in the memory reservation map.
  
   All calls to this function should be done before beginning to build
   the structure (nodes and properties)

   @param writer A writer opaque structure
   @param addr Reserve map base address
   @param size Reserve map size
 */
void fdt_writer_add_rsvmap(
	struct fdt_writer_s *writer,
	uint64_t addr,
	uint64_t size);

/**
   @this pushes a node in the device tree.

   Only the root node should have an empty name ("").

   @param writer A writer opaque structure
   @param name The relative name of the new node, without '/'
   @return The offset of the node in the structure
 */
uint32_t fdt_writer_node_entry(
	struct fdt_writer_s *writer,
	const char *name);

/**
   @this pushes an property in the current node. The data will be
   copied in the blob.

   @param writer A writer opaque structure
   @param name The property name
   @param data The property value, may be NULL if len is 0
   @param len The property size, in bytes
 */
void fdt_writer_node_prop(
	struct fdt_writer_s *writer,
	const char *name,
	void *data,
	size_t len);

/**
   @this closes the current node.

   @param writer A writer opaque structure
 */
void fdt_writer_node_leave(struct fdt_writer_s *writer);

/**
   @this finalizes the blob. This function packs the data and writes
   all the offsets.

   @param writer A writer opaque structure
   @param real_size A pointer to a variable to fill with the real size
   taken by the packed blob, if successful
   @return whether finalization succeded
 */
error_t fdt_writer_finalize(struct fdt_writer_s *writer, size_t *real_size);

#endif
