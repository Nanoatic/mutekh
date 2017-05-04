
SUBDIRS = termui

lib_LTLIBRARIES = libtermui.la

libtermui_la_SOURCES =	term.c term_none.c term_printf.c	\
			term_vt100.c term_vt102.c \
			term_xterm.c term_pv.h strings.c

if COND_telnet
libtermui_la_SOURCES += term_telnet.c 
endif

if COND_console
libtermui_la_SOURCES += console.c console_parse.c console_entries.c	\
			console_builtin.c console_process.c console_pv.h
endif

if COND_console_completion
libtermui_la_SOURCES += console_complete.c console_complete_readdir.c
endif

if COND_getline
libtermui_la_SOURCES += getline.c getline_complete.c	\
			getline_edit.c getline_history.c getline_pv.h
endif

libtermui_la_LDFLAGS = -version-info 1:0:0 $(libtool_flags)

