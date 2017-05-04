#include <hexo/init.h>
#include <hexo/segment.h>
#include <hexo/types.h>
#include <hexo/interrupt.h>
#if defined(CONFIG_HEXO_IPI)
#include <hexo/ipi.h>
#endif

#include <drivers/enum/fdt/enum-fdt.h>
#include <device/icu.h>
#include <device/device.h>
#include <device/driver.h>

#include <mutek/printk.h>

#include <fdt/reader.h>

#if 0
#define dprintk(...) printk(__VA_ARGS__)
#else
#define dprintk(...) do{}while(0)
#endif

#ifdef CONFIG_MUTEK_CONSOLE
extern struct device_s *console_dev;
#endif

#ifdef CONFIG_VFS
extern struct device_s *root_dev;
#endif

#if defined (CONFIG_MUTEK_TIMERMS)
extern struct device_s *timerms_dev;
#endif

enum state_e
{
	IN_ROOT,
	IN_CHOSEN,
	IN_CPUS,
	IN_CPU,
	IN_TOPOLOGY,
};

struct consumer_state_s
{
	struct device_s *enum_dev;
	enum state_e state;
#if defined(CONFIG_HEXO_IPI)
	struct device_s *ipi_dev;
	uint32_t ipi_no;
#endif
	uint32_t cpuid;
};

static FDT_ON_NODE_ENTRY_FUNC(creator_node_entry)
{
	struct consumer_state_s *private = priv;

	dprintk(">> %s, %d\n", path, private->state);

	switch (private->state) {
	case IN_ROOT:
		if ( !strcmp(path, "/chosen") ) {
			private->state = IN_CHOSEN;
			return 1;
		}
		if ( !strcmp(path, "/cpus") ) {
			private->state = IN_CPUS;
			return 1;
		}
		if ( !strcmp(path, "/topology") ) {
			private->state = IN_TOPOLOGY;
			return 1;
		}
		break;
	case IN_CPUS: {
		private->state = IN_CPU;
#if defined(CONFIG_HEXO_IPI)
		private->ipi_dev = NULL;
		private->ipi_no = 0;
#endif

#if defined(CONFIG_ARCH_SMP)
		bool_t got;
		got = fdt_reader_get_prop_int(
			state, "reg",
			&private->cpuid, sizeof(private->cpuid));
		assert(got);

		void *cls = arch_cpudata_alloc();
		dprintk("  created CLS @ %p\n", cls);
		cpu_local_storage[private->cpuid] = cls;
#endif
		break;
	}
	default:
		break;
	}
			
	return 1;
}

static FDT_ON_NODE_LEAVE_FUNC(creator_node_leave)
{
	struct consumer_state_s *private = priv;

	dprintk("<< %d\n", private->state);

	switch (private->state) {
	case IN_ROOT:
		break;
	case IN_CPUS:
	case IN_CHOSEN:
	case IN_TOPOLOGY:
		private->state = IN_ROOT;
		break;
	case IN_CPU:
		private->state = IN_CPUS;
#if defined(CONFIG_HEXO_IPI)
		if ( private->ipi_dev && private->ipi_dev->drv ) {
		  dev_icu_setup_ipi_ep( private->ipi_dev, 
					CPU_LOCAL_CLS_ADDR(cpu_local_storage[private->cpuid], ipi_endpoint),
					private->ipi_no);
		} else {
			dprintk("  No IPI dev for CPU %d\n", private->cpuid);
		}
#endif
		break;
	}
}

static FDT_ON_NODE_PROP_FUNC(creator_node_prop)
{
	struct consumer_state_s *private = priv;

	switch (private->state) {
	case IN_CHOSEN:
#ifdef CONFIG_MUTEK_CONSOLE
		if ( !strcmp(name, "console") ) {
			struct device_s *cd = enum_fdt_lookup(private->enum_dev, data);
			if ( cd && cd->drv ) {
				dprintk("Setting console device to node %s\n", data);
				console_dev = cd;
			}
		}
#endif
#ifdef CONFIG_MUTEK_TIMERMS
		if ( !strcmp(name, "timer") ) {
			struct device_s *cd = enum_fdt_lookup(private->enum_dev, data);
			if ( cd && cd->drv ) {
				dprintk("Setting timer device to node %s\n", data);
				timerms_dev = cd;
			}
		}
#endif
#ifdef CONFIG_VFS
		if ( !strcmp(name, "root") ) {
			struct device_s *cd = enum_fdt_lookup(private->enum_dev, data);
			if ( cd && cd->drv ) {
				dprintk("Setting root device to node %s\n", data);
				root_dev = cd;
			}
		}
#endif
		break;
	case IN_CPU:
#if defined(CONFIG_HEXO_IPI)
		if ( !strcmp(name, "ipi_dev") ) {
			printk("Warning: ipi_dev/ipi_no couple got deprecated in favor of ipi = <&{/dev} ipi_no>\n");
			private->ipi_dev = enum_fdt_lookup(private->enum_dev, data);
			dprintk("Getting ipi dev for cpu %d \"%s\": %p\n", private->cpuid, data, private->ipi_dev);
		} else if ( !strcmp(name, "ipi_no") ) {
			printk("Warning: ipi_dev/ipi_no couple got deprecated in favor of ipi = <&{/dev} ipi_no>\n");
			fdt_parse_sized(1, data, sizeof(private->ipi_no), &private->ipi_no);
			dprintk("Getting ipi no for cpu %d: %d\n", private->cpuid, private->ipi_no);
		} else if ( !strcmp(name, "ipi") ) {
			uint32_t phandle;
			void *no;
			no = fdt_parse_sized(1, data, sizeof(phandle), &phandle);
			fdt_parse_sized(1, no, sizeof(&private->ipi_no), &private->ipi_no);
			private->ipi_dev = enum_fdt_get_phandle(private->enum_dev, phandle);
			dprintk("Getting ipi dev for cpu %d \"%s\": %p\n", private->cpuid, data, private->ipi_dev);
		}
#endif
		break;
	default:
		break;
	}
}

static FDT_ON_MEM_RESERVE_FUNC(creator_mem_reserve)
{
}


void mutek_parse_fdt(struct device_s *enum_dev, void *blob)
{
	struct consumer_state_s priv = {
		.enum_dev = enum_dev,
		.state = IN_ROOT,
	};

	struct fdt_walker_s walker = {
		.priv = &priv,
		.on_node_entry = creator_node_entry,
		.on_node_leave = creator_node_leave,
		.on_node_prop = creator_node_prop,
		.on_mem_reserve = creator_mem_reserve,
	};

	dprintk("%s walking blob\n", __FUNCTION__);
	fdt_walk_blob(blob, &walker);
}
