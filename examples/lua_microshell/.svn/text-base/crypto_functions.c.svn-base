
#include <stdio.h>
#include <errno.h>
#include <mutek/printk.h>
#include <lua/lua.h>

#include <crypto/md5.h>

int md5(lua_State *st)
{
    unsigned int i;

    for (i = 1; i <= lua_gettop(st); i++)
    {
        switch (lua_type(st, i))
        {
        case LUA_TSTRING:
        {
            FILE* f;
            const char *pathname = lua_tolstring(st, i, NULL);
            uint8_t buffer[256];
            ssize_t s;
            struct crypto_md5_ctx_s hash;
            uint8_t digest[16];

            if ( pathname == NULL )
                return 0;

            crypto_md5_init(&hash);

            if ((f = fopen(pathname, "r")) == NULL)
            {
                printk("error '%s': %s\n", pathname, strerror(errno));
                break;
            }

            while ((s = fread(buffer, 1, sizeof(buffer), f)) > 0) {
                crypto_md5_update(&hash, buffer, s);
            }
            
            crypto_md5_get(&hash, digest);
            printk("md5: %P\n", digest, 16);

            fclose(f);
            break;
        }
        default:
            printk("bad argument\n");
            break;
        }
    }

    return 0;
}

void init_crypto_shell(lua_State* luast)
{
    lua_register(luast, "md5", md5);
}
