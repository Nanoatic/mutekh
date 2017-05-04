/*
 * This file is part of MutekH.
 * 
 * MutekH is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 of the License.
 * 
 * MutekH is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with MutekH; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * Copyright (c) UPMC, Lip6, SoC
 *         Nicolas Pouillon <nipo@ssji.net>, 2008
 */

#include <string.h>

#include <hexo/local.h>
#include <hexo/types.h>
#include <hexo/iospace.h>
#include <hexo/endian.h>

#include <libc/formatter.h>

static PRINTF_OUTPUT_FUNC(srl_log_out)
{
	uintptr_t out_addr = (uintptr_t)ctx;
	if (!out_addr)
		return;

	while (len--) {
		cpu_mem_write_32(out_addr, endian_le32(*str++));
	}
}

static void *tcg_tty;

#if defined(CONFIG_SRL_MULTI_TTY)

static CPU_LOCAL void *cpu_tty;
static CONTEXT_LOCAL void *context_tty;

void srl_console_init(void *addr)
{
	tcg_tty = addr;
}

void srl_console_init_cpu(void *addr)
{
	CPU_LOCAL_SET(cpu_tty, addr ? addr : tcg_tty);
}

void srl_console_init_task(void *addr)
{
	CONTEXT_LOCAL_SET(context_tty, addr ? addr : tcg_tty);
}

void _srl_log(const char *str)
{
	srl_log_out(CONTEXT_LOCAL_GET(context_tty), str, 0, strlen(str));
}

void _srl_log_printf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	formatter_printf(CONTEXT_LOCAL_GET(context_tty), srl_log_out, fmt, ap);
	va_end(ap);
}

void _cpu_printf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	formatter_printf(CPU_LOCAL_GET(cpu_tty), srl_log_out, fmt, ap);
	va_end(ap);
}

#else // not CONFIG_SRL_MULTI_TTY

void srl_console_init(void *addr)
{
	tcg_tty = addr;
}

void srl_console_init_cpu(void *addr)
{
}

void srl_console_init_task(void *addr)
{
}

void _srl_log(const char *str)
{
	srl_log_out(tcg_tty, str, 0, strlen(str));
}

void _srl_log_printf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	formatter_printf(tcg_tty, srl_log_out, fmt, ap);
	va_end(ap);
}

void _cpu_printf(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	formatter_printf(tcg_tty, srl_log_out, fmt, ap);
	va_end(ap);
}

#endif // end CONFIG_SRL_MULTI_TTY
