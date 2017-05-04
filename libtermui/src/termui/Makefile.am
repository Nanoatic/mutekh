
pkgincludedir = $(includedir)/termui
pkginclude_HEADERS = term.h bhv.h term_keys.h termui_config.h

if COND_console
pkginclude_HEADERS += console.h
endif

if COND_getline
pkginclude_HEADERS += getline.h 
endif

termui_config.h: $(top_builddir)/config.h
	grep CONFIG_LIBTERMUI $(top_builddir)/config.h > $@

