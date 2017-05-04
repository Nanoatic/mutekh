#include <hexo/types.h>

__attribute__((section ("__DATA, __contextdata")))
uint32_t __context_data_end = 0;

__attribute__((section ("__DATA, __drivers")))
uint32_t global_driver_registry_end;
