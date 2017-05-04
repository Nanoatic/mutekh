
#include <hexo/types.h>
#include <stdio.h>
#include <errno.h>
#include <mutek/mem_alloc.h>
#include <mutek/printk.h>
#include <lua/lua.h>
#include <vfs/vfs.h>

#include <pthread.h>
#include <libelf/rtld.h>


int _exec(lua_State *st)
{
    pthread_t b;
    const char *filename = lua_tostring(st, 1);
    const char *func = lua_tostring(st,2);

    struct dynobj_rtld_s *obj;
    uintptr_t entrypoint;

    if (rtld_open(&obj, filename) != 0)
    {
        printk("dlopen failed on %s...\n", filename);
        return 0;
    }
    else if (rtld_sym(obj, func, &entrypoint) != 0)
    {
        printk("dlsym failed on %s...\n", func);
        return 0;
    }

    printk("\x1b[31m");

    pthread_create(&b, NULL, (pthread_start_routine_t*)entrypoint, NULL);

    void *ret;
    pthread_join(b, &ret);

    printk("\x1b[0m");
    printk("Run finished !\n");
    return 0;
}

void init_rtld_shell(lua_State* luast)
{
    printk("init rtld...");
    rtld_init();
    printk("ok\n");

    lua_register(luast, "exec", _exec);
}
