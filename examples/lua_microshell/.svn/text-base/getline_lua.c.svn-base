#include <stdio.h>
#include <mutek/printk.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <device/device.h>
#include <device/driver.h>
#include <hexo/interrupt.h>

#if defined(CONFIG_VFS)
# include <vfs/vfs.h>
#endif

#include <lua/lauxlib.h>
#include <lua/lua.h>

#include <termui/term.h>
#include <termui/getline.h>

extern struct device_s * console_dev;

pthread_t a;

void init_mem_shell(lua_State*);
void init_vfs_shell(lua_State*);
void init_rtld_shell(lua_State* luast);
void init_crypto_shell(lua_State* luast);
void init_timer_shell(lua_State *st);

static void initialize_shell(lua_State* luast)
{
    init_mem_shell(luast);

#if defined(CONFIG_VFS)
    init_vfs_shell(luast);
#endif

#if defined(CONFIG_LIBELF_RTLD)
    init_rtld_shell(luast);
#endif

#if defined(CONFIG_LIBCRYPTO_MD5)
    init_crypto_shell(luast);
#endif

#if defined(CONFIG_MUTEK_TIMERMS)
    init_timer_shell(luast);
#endif

#if defined(CONFIG_DRIVER_LCD)
    init_lcd_shell(luast);
#endif
}





/* line completion handler found in getline_lua_complete.c */
TERMUI_GETLINE_FCN_COMPLETE(lua_complete);

static TERMUI_GETLINE_FCN_PROMPT(prompt)
{
#if defined(CONFIG_VFS)
    char name[CONFIG_VFS_NAMELEN];
    vfs_node_get_name(vfs_get_cwd(), name, CONFIG_VFS_NAMELEN);
    return termui_term_printf(tm, "[lua:%s] ", name);
#else
    return termui_term_printf(tm, "[lua] ");
#endif
}

void* shell(void *param)
{
    struct termui_term_s		*tm;
    struct termui_term_behavior_s	*bhv;
    lua_State				*luast;

    /* create lua state */
    luast = luaL_newstate();

    luaL_openlibs(luast);

    initialize_shell(luast);

    /* initialize terminal */
    if (!(tm = termui_term_alloc(console_dev, console_dev, luast)))
        return NULL;

    /* set capabilities */
    termui_term_set(tm, "xterm");

#if defined(CONFIG_DRIVER_CHAR_SOCLIBTTY)
    char *disable_cr = "\x1b[20l";
    char *enable_cr = "\x1b[20h";
    termui_term_writestr(tm, disable_cr, strlen(disable_cr));
#endif

    termui_term_printf(tm, "lua shell example, use Ctrl-D to quit\n\n");

    /* initialize getline behavior according to term capabilities */
    if (!(bhv = termui_getline_alloc(tm, 256)))	/* max line len = 256 */
        return NULL;

    termui_getline_history_init(bhv, 64); /* 64 entries max */
    termui_getline_complete_init(bhv, lua_complete);
    termui_getline_setprompt(bhv, prompt);

    while (1)
    {
        int oldtop;
        const char *line;

        if (!(line = termui_getline_process(bhv)))
            break;

        /* skip blank line */
        if (!*(line += strspn(line, "\n\r\t ")))
            continue;

        termui_getline_history_addlast(bhv);

        if (luaL_loadbuffer(luast, line, strlen(line), ""))
        {
            termui_term_printf(tm, "%91AParse error:%A %s\n", lua_tostring(luast, -1));
            lua_pop(luast, 1);
            continue;
        }

        oldtop = lua_gettop(luast);

#if defined(CONFIG_DRIVER_CHAR_SOCLIBTTY)
        termui_term_writestr(tm, enable_cr, strlen(enable_cr));
#endif
        int err = lua_pcall(luast, 0, LUA_MULTRET, 0);
#if defined(CONFIG_DRIVER_CHAR_SOCLIBTTY)
        termui_term_writestr(tm, disable_cr, strlen(disable_cr));
#endif

        if (err)
        {
            termui_term_printf(tm, "%91AExecution error:%A %s\n", lua_tostring(luast, -1));
            lua_pop(luast, 1);
            continue;
        }

        lua_pop(luast, lua_gettop(luast) - oldtop + 1);
    }

    lua_close(luast);

    /* free resources allocated by getline behavior */
    termui_getline_free(bhv);

    /* free resources and restore terminal attributes */
    termui_term_free(tm);

    return 0;
}


void app_start()
{
    pthread_create(&a, NULL, shell, NULL);
}

