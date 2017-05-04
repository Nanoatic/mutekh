/*
    This file is part of MutekH.

    MutekH is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    MutekH is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MutekH; if not, write to the Free Software Foundation,
    Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

    Copyright Alexandre Becoulet <alexandre.becoulet@lip6.fr> (c) 2006

*/


#include <hexo/types.h>
#include <device/char.h>
#include <device/device.h>
#include <device/driver.h>

#include <string.h>

#include "tty-vga.h"

#include "tty-vga-private.h"

/**************************************************************/

#ifdef CONFIG_DRIVER_CHAR_VGATTY_ANSI

/* 
 * put ansi reponse data on device read fifo
 */

#define VGA_TTY_ANSI_PUTSTR(fifo, str) tty_fifo_pushback_array(fifo, (uint8_t*)str, sizeof(str) - 1)

static void
tty_vga_ansi_putint(tty_fifo_root_t *fifo, uint_fast8_t n)
{
  if (n > 10)
    tty_vga_ansi_putint(fifo, n / 10);
  tty_fifo_pushback(fifo, (n % 10) + '0');
}

/* 
 * set term attributes
 */

static void inline
tty_vga_ansi_attr(struct tty_vga_context_s *pv, uint_fast8_t param)
{
  switch (param)
    {
    case (0):
      pv->forecolor = 7;
      pv->backcolor = 0;
      pv->blink = 0;
      pv->bright = 0;
      pv->reverse = 0;
      break;

    case (4):			/* underscore */
    case (1):			/* bright */
      pv->bright = 1;
      break;

    case (2):			/* Dim */
      break;

    case (5):			/* blink */
      pv->blink = 1;
      break;

    case (7):			/* reverse */
      pv->reverse = 1;
      break;

    case (8):			/* invisible */
      pv->forecolor = pv->backcolor;
      break;

      /* foreground color */
    case (30): pv->forecolor = 0; break;
    case (31): pv->forecolor = 4; break;
    case (32): pv->forecolor = 2; break;
    case (33): pv->forecolor = 6; break;
    case (34): pv->forecolor = 1; break;
    case (35): pv->forecolor = 5; break;
    case (36): pv->forecolor = 3; break;
    case (37): pv->forecolor = 7; break;

      /* background color */
    case (40): pv->backcolor = 0; break;
    case (41): pv->backcolor = 4; break;
    case (42): pv->backcolor = 2; break;
    case (43): pv->backcolor = 6; break;
    case (44): pv->backcolor = 1; break;
    case (45): pv->backcolor = 5; break;
    case (46): pv->backcolor = 3; break;
    case (47): pv->backcolor = 7; break;
    }
}

/*
 * delete n characters at cursor position and shift left
 */

static void
tty_vga_ansi_delete(struct device_s *dev, int_fast8_t count)
{
  vga_text_buf_t		buf = (vga_text_buf_t)(dev->addr[VGA_TTY_ADDR_BUFFER]);
  struct tty_vga_context_s	*pv = dev->drv_pv;
  int_fast16_t	i;

  buf += pv->width * pv->ypos;

  for (i = pv->xpos; i < pv->width - count; i++)
    buf[i] = buf[i + count];

  for (; i < pv->width; i++)
    {
      buf[i].c = ' ';
      buf[i].attrs = buf[pv->width - 1].attrs;
    }
}

void
tty_vga_ansi_insert(struct device_s *dev)
{
  vga_text_buf_t		buf = (vga_text_buf_t)(dev->addr[VGA_TTY_ADDR_BUFFER]);
  struct tty_vga_context_s	*pv = dev->drv_pv;
  int_fast16_t	i;

  buf += pv->width * pv->ypos;

  for (i = pv->width - 1; i > pv->xpos; i--)
    buf[i] = buf[i - 1];
}


/* 
 * process as ansi code char after '?'
 */

static void
tty_vga_process_ansi_qmark(struct device_s *dev, uint8_t c)
{
  struct tty_vga_context_s	*pv = dev->drv_pv;
  uint_fast8_t	i;

  switch (c)
    {
    case ('l'):
      for (i = 0; i <= pv->ansi_index; i++)
	switch (pv->ansi_param[i])
	  {
	  }
      break;

    case ('h'):
      for (i = 0; i <= pv->ansi_index; i++)
	switch (pv->ansi_param[i])
	  {
	  }
      break;

      /* param digits */
    case '0'...'9':
      pv->ansi_param[pv->ansi_index] = pv->ansi_param[pv->ansi_index] * 10 + c - '0';
      return;

    case (';'):			/* param separator */
      pv->ansi_index = (pv->ansi_index + 1) % VGA_TTY_MAX_ANSI_PARAMS;
      return;
    }

  pv->process = &tty_vga_process_default;
}

/* 
 * process as ansi code char after '['
 */

static void
tty_vga_process_ansi_charset(struct device_s *dev, uint8_t c)
{
  struct tty_vga_context_s	*pv = dev->drv_pv;

  /* Select charset */

  pv->process = &tty_vga_process_default;
}

/* 
 * process as ansi code char after '['
 */

static void
tty_vga_process_ansi_bracket(struct device_s *dev, uint8_t c)
{
  struct tty_vga_context_s	*pv = dev->drv_pv;
  uint_fast8_t	i;

  switch (c)
    {
    case(27):		/* ESC */
      pv->process = &tty_vga_process_ansi;
      return;

    case ('c'):			/* Query Device Code, report as vt102*/
      VGA_TTY_ANSI_PUTSTR(&pv->read_fifo, "\x1b[?6c");
      break;

    case ('n'):

      switch (pv->ansi_param[0])
	{
	case (5):		/* Query Device Status */
	  VGA_TTY_ANSI_PUTSTR(&pv->read_fifo, "\x1b[0n");
	  break;

	case (6):		/* Query Cursor Position */
	  VGA_TTY_ANSI_PUTSTR(&pv->read_fifo, "\x1b[");
	  tty_vga_ansi_putint(&pv->read_fifo, pv->xpos + 1);
	  VGA_TTY_ANSI_PUTSTR(&pv->read_fifo, ";");
	  tty_vga_ansi_putint(&pv->read_fifo, pv->ypos + 1);
	  VGA_TTY_ANSI_PUTSTR(&pv->read_fifo, "R");
	  break;
	}

      break;

    case ('?'):
      pv->process = &tty_vga_process_ansi_qmark;
      return;

    case ('l'):
      for (i = 0; i <= pv->ansi_index; i++)
	switch (pv->ansi_param[i])
	  {
	  case (4):
	    pv->insert = 0;	/* Disable char insert */
	    break;
	  case (7):		/* Disable line wrap */
	    pv->linewrap = 0;
	    break;
	  case (20):		/* Disable newline mode */
	    pv->nlmode = 0;
	    break;
	  }
      break;

    case ('h'):
      for (i = 0; i <= pv->ansi_index; i++)
	switch (pv->ansi_param[i])
	  {
	  case (4):
	    pv->insert = 1;	/* Disable char insert */
	    break;
	  case (7):		/* Enable line wrap */
	    pv->linewrap = 1;
	    break;
	  case (20):		/* Enable newline mode */
	    pv->nlmode = 1;
	    break;
	  }
      break;

    case ('m'):			/* attributes change */
      for (i = 0; i <= pv->ansi_index; i++)
	tty_vga_ansi_attr(pv, pv->ansi_param[i]);
      break;

    case ('f'):			/* Force Cursor Position */
    case ('H'):			/* Cursor Home */
      tty_vga_setcursor(dev, (int_fast8_t)pv->ansi_param[1] - 1,
			(int_fast8_t)pv->ansi_param[0] - 1);
      break;

    case ('A'):			/* Move Up */
      i = pv->ansi_param[0];
      tty_vga_setcursor(dev, pv->xpos, pv->ypos - (i ? i : 1));
      break;

    case ('B'):			/* Move Down */
      i = pv->ansi_param[0];
      tty_vga_setcursor(dev, pv->xpos, pv->ypos + (i ? i : 1));
      break;

    case ('C'):			/* Move Forward */
      i = pv->ansi_param[0];
      tty_vga_setcursor(dev, pv->xpos + (i ? i : 1), pv->ypos);
      break;

    case ('D'):			/* Move Backward */
      i = pv->ansi_param[0];
      tty_vga_setcursor(dev, pv->xpos - (i ? i : 1), pv->ypos);
      break;

    case ('s'):			/* Save cursor position */
      pv->xsave = pv->xpos;
      pv->ysave = pv->ypos;
      break;

    case ('u'):			/* Restore cursor postion */
      tty_vga_setcursor(dev, pv->xsave, pv->ysave);
      break;

    case ('r'):
      /* FIXME scroll region not handled */
      break;

    case ('g'):
      /* FIXME tab not handled */
      break;

    case ('K'):
      switch (pv->ansi_param[0])
	{
	case (0):		/* Clear end of line */
	  tty_vga_clear_row(dev, pv->ypos, pv->xpos, pv->width);
	  break;

	case (1):		/* Clear upto cursor */
	  tty_vga_clear_row(dev, pv->ypos, 0, pv->xpos + 1);
	  break;

	case (2):		/* Clear line */
	  tty_vga_clear_row(dev, pv->ypos, 0, pv->width);
	  break;
	}
      break;

    case ('J'):
      switch (pv->ansi_param[0])
	{
	case (0):		/* Clear end of screen */
	  tty_vga_clear_row(dev, pv->ypos, pv->xpos, pv->width);
	  tty_vga_clear(dev, pv->ypos + 1, pv->height);
	  break;

	case (1):		/* Clear screen upto cursor */
	  tty_vga_clear(dev, 0, pv->ypos);
	  tty_vga_clear_row(dev, pv->ypos, 0, pv->xpos + 1);
	  break;

	case (2):		/* Clear screen */
	  tty_vga_clear(dev, 0, pv->height);
	  break;
	}
      break;

    case ('P'):
      tty_vga_ansi_delete(dev, pv->ansi_param[0]);
      break;

    case ('L'):			/* Insert Line(s) */
      /* FIXME add support */
      break;

    case ('M'):			/* Delete Line(s) */
      /* FIXME add support */
      break;

      /* param digits */
    case '0' ... '9':
      pv->ansi_param[pv->ansi_index] = pv->ansi_param[pv->ansi_index] * 10 + c - '0';
      return;

    case (';'):			/* param separator */
      pv->ansi_index = (pv->ansi_index + 1) % VGA_TTY_MAX_ANSI_PARAMS;
      return;
    }

  pv->process = &tty_vga_process_default;
}

/* 
 * process as ansi code first char
 */

void
tty_vga_process_ansi(struct device_s *dev, uint8_t c)
{
  struct tty_vga_context_s	*pv = dev->drv_pv;

  pv->ansi_index = 0;
  memset(pv->ansi_param, 0, sizeof (pv->ansi_param));

  switch (c)
    {
    case(27):			/* ESC */
      return;

    case ('('): case (')'): case ('*'): case ('+'):
      pv->process = &tty_vga_process_ansi_charset;
      return;

    case ('['):
      pv->process = &tty_vga_process_ansi_bracket;
      return;

    case ('c'):			/* Reset Device */
      tty_vga_reset(dev);
      break;

    case ('7'):			/* Save cursor position & attr*/
      /* FIXME attributes should be saved here */
      pv->xsave = pv->xpos;
      pv->ysave = pv->ypos;
      break;

    case ('8'):			/* Restore cursor postion & attr */
      /* FIXME attributes should be restored here */
      tty_vga_setcursor(dev, pv->xsave, pv->ysave);
      break;

    case ('D'):			/* Scroll down */
      if (tty_vga_setcursor(dev, pv->xpos, pv->ypos + 1))
	tty_vga_scroll_up(dev, 1);
      break;

    case ('M'):			/* Scroll up */
      if (tty_vga_setcursor(dev, pv->xpos, pv->ypos - 1))
	tty_vga_scroll_down(dev, 1);
      break;

    case ('E'):			/* Next Line */
      if (tty_vga_setcursor(dev, 0, pv->ypos + 1))
	tty_vga_scroll_up(dev, 1);

    case ('H'):
      /* FIXME tab not handled */
      break;
   }

  pv->process = &tty_vga_process_default;
}

#endif

