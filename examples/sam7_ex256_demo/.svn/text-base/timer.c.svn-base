
#include <mutek/timer.h>
#include <mutek/mem_alloc.h>
#include <mutek/semaphore.h>

#include <lua/lauxlib.h>
#include <lua/lua.h>

#include <stdio.h>
#include <string.h>

static struct timer_event_s sleep_event;
static struct semaphore_s timer_sem;

static TIMER_CALLBACK(restart)
{
	semaphore_give(&timer_sem, 1);
}

static TIMER_CALLBACK(delayed_print)
{
	printk("%s\n", pv);
	mem_free(pv);
	mem_free(timer);
}

int cmd_usleep(lua_State *st)
{
	if ( lua_gettop(st) < 1 )
		return 1;

	uint32_t usec = lua_tonumber(st, 1);
	sleep_event.delay = usec;
	timer_add_event(&timer_ms, &sleep_event);
	semaphore_take(&timer_sem, 1);
	return 0;
}

int cmd_printlater(lua_State *st)
{
	if ( lua_gettop(st) < 2 )
		return 1;

	struct timer_event_s *print_event = mem_alloc(sizeof(*print_event), mem_scope_sys);

	uint32_t usec = lua_tonumber(st, 1);
	const char *str = lua_tostring(st, 2);
	print_event->callback = delayed_print;
	print_event->delay = usec;
	print_event->pv = strdup(str);
	timer_add_event(&timer_ms, print_event);
	return 0;
}

void term_timer_init(lua_State *st)
{
	semaphore_init(&timer_sem, 0);

	sleep_event.callback = restart;
	sleep_event.pv = sched_get_current();

	lua_register(st, "printlater", cmd_printlater);
	lua_register(st, "usleep", cmd_usleep);
}
