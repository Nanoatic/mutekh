#include <mutek/mem_alloc.h>
#include <string.h>
#include <stdlib.h>

inline void *
malloc(size_t size)
{
  return mem_alloc(size, (mem_scope_default));
}

void *
calloc(size_t nmemb, size_t size)
{
  void	*ptr;

  if ((ptr = malloc(nmemb * size)))
    memset(ptr, 0, nmemb * size);

  return ptr;
}

void
free(void *ptr)
{
  if (ptr != NULL)
    mem_free(ptr);
}

void *
realloc(void *ptr, size_t size)
{
  size_t	oldsize;
  void		*p;

  if (ptr == NULL)
    return malloc(size);

  if (size == 0)
    {
      free(ptr);
      return NULL;
    }

  oldsize = mem_getsize(ptr);

  if (oldsize == size)
    return ptr;

  if( ( p = mem_resize(ptr, size) ) != NULL )
    return p;

  if( ! (p = malloc(size)))
    return NULL;

  /*Memchecker must be disable for init checking. Initial area may
    contains uninitialized words.*/
#ifdef CONFIG_SOCLIB_MEMCHECK
  soclib_mem_check_disable(SOCLIB_MC_CHECK_INIT);
#endif
  //  memcpy(p, ptr, oldsize );
#ifdef CONFIG_SOCLIB_MEMCHECK
  soclib_mem_check_enable(SOCLIB_MC_CHECK_INIT);
#endif

  free(ptr);
  return p;
}

