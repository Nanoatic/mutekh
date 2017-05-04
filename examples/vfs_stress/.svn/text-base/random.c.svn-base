#include <hexo/types.h>
#include <hexo/local.h>

THREAD_LOCAL uint16_t my_rand_state;

uint16_t my_rand(uint16_t i)
{
	uint16_t r = TLS_GET(my_rand_state);
	r = (r >> 1) ^ (-(r & 1) & 0xB400u);
	TLS_SET(my_rand_state, r);
	return r;
}

void my_rand_init(uint16_t i)
{
	TLS_SET(my_rand_state, i);
}
