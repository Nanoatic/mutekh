MKDOC=mkdoc

include $(MUTEK_SRC_DIR)/doc/header_list.mk

include $(MUTEK_SRC_DIR)/scripts/discover.mk

SVN_REV:=$(shell svn info $(MUTEK_SRC_DIR) | grep ^Revision: | cut -d' ' -f2)

$(BUILD_DIR)/doc/config.h:
	test -d $(BUILD_DIR)/doc || mkdir -p $(BUILD_DIR)/doc
	perl $(MUTEK_SRC_DIR)/scripts/config.pl --docheader=$@

doc: $(BUILD_DIR)/doc/config.h
	cd $(MUTEK_SRC_DIR) ; \
	$(MKDOC) $(MKDOCFLAGS) doc/gpct.mkdoclib \
	  --mkdoclib-create doc/html/mutek-api \
	  --mkdoclib-url http://www.mutek.fr/www/mutekh_api/ \
	  --output-path $(BUILD_DIR)/doc \
	  --source-rev $(SVN_REV) \
	  -I $(BUILD_DIR) doc/config.h \
	  -I . \
	  -I doc/include \
	  $(subst $(MUTEK_SRC_DIR)/,,$(MKDOC_ARGS)) \
	  $(CPU_HEADER) $(ARCH_HEADER)

.PHONY: doc
