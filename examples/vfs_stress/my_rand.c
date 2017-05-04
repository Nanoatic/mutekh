#include <hexo/types.h>
#include <hexo/local.h>

CONTEXT_LOCAL uint16_t my_rand_state = 0;

uint16_t my_rand()
{
	uint16_t r = CONTEXT_LOCAL_GET(my_rand_state);
	r = (r >> 1) ^ ((-(r & 1)) & 0xB400u);
	CONTEXT_LOCAL_SET(my_rand_state, r);
//    printk("Rand: %d\n", r);
	return r;
}

void my_rand_init(uint16_t i)
{
	CONTEXT_LOCAL_SET(my_rand_state, i);
}
