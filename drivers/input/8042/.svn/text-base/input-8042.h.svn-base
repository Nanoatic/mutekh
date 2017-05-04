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

#ifndef DRIVER_INPUT_8042_H_
#define DRIVER_INPUT_8042_H_

#include <device/input.h>
#include <device/device.h>

/* devices addresses slots */

#define INPUT_8042_ADDR	0

/* input device functions */

DEV_IRQ(input_8042_irq);
DEV_INIT(input_8042_init);
DEV_CLEANUP(input_8042_cleanup);
DEVINPUT_INFO(input_8042_info);
DEVINPUT_READ(input_8042_read);
DEVINPUT_WRITE(input_8042_write);
DEVINPUT_SETCALLBACK(input_8042_setcallback);

/* PC keyboard keys */

#define INPUT_8042_KEYCOUNT		128

	/* The keyboard syms have been cleverly chosen to map to ASCII */
#define INPUT_8042_KEY_BACKSPACE	8
#define INPUT_8042_KEY_TAB		9
#define INPUT_8042_KEY_CLEAR		12
#define INPUT_8042_KEY_RETURN		13
#define INPUT_8042_KEY_PAUSE		19
#define INPUT_8042_KEY_ESCAPE		27
#define INPUT_8042_KEY_SPACE		32
#define INPUT_8042_KEY_EXCLAIM		33
#define INPUT_8042_KEY_QUOTEDBL		34
#define INPUT_8042_KEY_HASH		35
#define INPUT_8042_KEY_DOLLAR		36
#define INPUT_8042_KEY_PERCENT		37
#define INPUT_8042_KEY_AMPERSAND	38
#define INPUT_8042_KEY_QUOTE		39
#define INPUT_8042_KEY_LEFTPAREN	40
#define INPUT_8042_KEY_RIGHTPAREN	41
#define INPUT_8042_KEY_ASTERISK		42
#define INPUT_8042_KEY_PLUS		43
#define INPUT_8042_KEY_COMMA		44
#define INPUT_8042_KEY_MINUS		45
#define INPUT_8042_KEY_PERIOD		46
#define INPUT_8042_KEY_SLASH		47
#define INPUT_8042_KEY_0		48
#define INPUT_8042_KEY_1		49
#define INPUT_8042_KEY_2		50
#define INPUT_8042_KEY_3		51
#define INPUT_8042_KEY_4		52
#define INPUT_8042_KEY_5		53
#define INPUT_8042_KEY_6		54
#define INPUT_8042_KEY_7		55
#define INPUT_8042_KEY_8		56
#define INPUT_8042_KEY_9		57
#define INPUT_8042_KEY_COLON		58
#define INPUT_8042_KEY_SEMICOLON	59
#define INPUT_8042_KEY_LESS		60
#define INPUT_8042_KEY_EQUALS		61
#define INPUT_8042_KEY_GREATER		62
#define INPUT_8042_KEY_QUESTION		63
#define INPUT_8042_KEY_AT		64

#define INPUT_8042_KEY_LEFTBRACKET	91
#define INPUT_8042_KEY_BACKSLASH	92
#define INPUT_8042_KEY_RIGHTBRACKET	93
#define INPUT_8042_KEY_CARET		94
#define INPUT_8042_KEY_BACKQUOTE	96
#define INPUT_8042_KEY_A		97
#define INPUT_8042_KEY_B		98
#define INPUT_8042_KEY_C		99
#define INPUT_8042_KEY_D		100
#define INPUT_8042_KEY_E		101
#define INPUT_8042_KEY_F		102
#define INPUT_8042_KEY_G		103
#define INPUT_8042_KEY_H		104
#define INPUT_8042_KEY_I		105
#define INPUT_8042_KEY_J		106
#define INPUT_8042_KEY_K		107
#define INPUT_8042_KEY_L		108
#define INPUT_8042_KEY_M		109
#define INPUT_8042_KEY_N		110
#define INPUT_8042_KEY_O		111
#define INPUT_8042_KEY_P		112
#define INPUT_8042_KEY_Q		113
#define INPUT_8042_KEY_R		114
#define INPUT_8042_KEY_S		115
#define INPUT_8042_KEY_T		116
#define INPUT_8042_KEY_U		117
#define INPUT_8042_KEY_V		118
#define INPUT_8042_KEY_W		119
#define INPUT_8042_KEY_X		120
#define INPUT_8042_KEY_Y		121
#define INPUT_8042_KEY_Z		122
#define INPUT_8042_KEY_DELETE		127

#define INPUT_8042_KEY_F1		0
#define INPUT_8042_KEY_F2		1
#define INPUT_8042_KEY_F3		2
#define INPUT_8042_KEY_F4		3
#define INPUT_8042_KEY_F5		4
#define INPUT_8042_KEY_F6		5
#define INPUT_8042_KEY_F7		6
#define INPUT_8042_KEY_F8		7
#define INPUT_8042_KEY_F9		10
#define INPUT_8042_KEY_F10		11
#define INPUT_8042_KEY_F11		14
#define INPUT_8042_KEY_F12		15

#define INPUT_8042_KEY_KP0		16
#define INPUT_8042_KEY_KP1		17
#define INPUT_8042_KEY_KP2		18
#define INPUT_8042_KEY_KP3		20
#define INPUT_8042_KEY_KP4		21
#define INPUT_8042_KEY_KP5		22
#define INPUT_8042_KEY_KP6		23
#define INPUT_8042_KEY_KP7		24
#define INPUT_8042_KEY_KP8		25
#define INPUT_8042_KEY_KP9		26
#define INPUT_8042_KEY_KP_PERIOD	28
#define INPUT_8042_KEY_KP_DIVIDE	29
#define INPUT_8042_KEY_KP_MULTIPLY	30
#define INPUT_8042_KEY_KP_MINUS		31
#define INPUT_8042_KEY_KP_PLUS		65
#define INPUT_8042_KEY_KP_ENTER		66

#define INPUT_8042_KEY_UP		67
#define INPUT_8042_KEY_DOWN		68
#define INPUT_8042_KEY_RIGHT		69
#define INPUT_8042_KEY_LEFT		70
#define INPUT_8042_KEY_INSERT		71
#define INPUT_8042_KEY_HOME		72
#define INPUT_8042_KEY_END		73
#define INPUT_8042_KEY_PAGEUP		74
#define INPUT_8042_KEY_PAGEDOWN		75

#define	INPUT_8042_KEY_NUMLOCK		76
#define INPUT_8042_KEY_CAPSLOCK		77
#define INPUT_8042_KEY_SCROLLOCK	78

#define INPUT_8042_KEY_RSHIFT		79
#define INPUT_8042_KEY_LSHIFT		80
#define INPUT_8042_KEY_RCTRL		81
#define INPUT_8042_KEY_LCTRL		82
#define INPUT_8042_KEY_RALT		83
#define INPUT_8042_KEY_LALT		84
#define INPUT_8042_KEY_LWIN  		85
#define INPUT_8042_KEY_POPUP		86
#define INPUT_8042_KEY_RWIN  		87
#define INPUT_8042_KEY_SYSREQ		88

#endif

