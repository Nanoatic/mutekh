
#include <pthread.h>
#include <mutek/printk.h>

#if defined(CONFIG_ARCH_EMU)
#include <arch/hexo/emu_syscalls.h>
#endif

#ifdef CONFIG_DRIVER_ENUM_ROOT
#include <device/enum.h>
#include <device/device.h>
#include <device/driver.h>

extern struct device_s enum_root;

static const char *device_class_str[] = {
	"none", "block", "char", "enum", "fb", "icu", "input", "net",
	"sound", "timer", "spi", "lcd", "gpio", "i2c", "mem",
};

static void dump_enumerator(struct device_s *root, uint_fast8_t prefix)
{
	uint_fast8_t i;
	for (i=0; i<prefix; ++i)
		printk(" ");
	printk("device %p, type %s\n", root,
		   root->drv ? device_class_str[root->drv->class] : "[undriven]");
	CONTAINER_FOREACH(device_list, CLIST, &root->children, {
			dump_enumerator(item, prefix+1);
		});
}
#endif

void app_start()
{
#ifdef CONFIG_DRIVER_ENUM_ROOT
	dump_enumerator(&enum_root, 0);
#endif

	printk("Demo ended\n");
#if defined(CONFIG_ARCH_EMU)
	emu_do_syscall(EMU_SYSCALL_EXIT, 1, 0);
#endif
}

