header_sources = $(wildcard $(LOCAL_SRC_DIR)/gpct/include/gpct/*.t)
header_raw = $(wildcard $(LOCAL_SRC_DIR)/gpct/include/gpct/*.h)

pre_headers = $(subst $(LOCAL_SRC_DIR)/gpct/,,$(header_sources:.t=.h))
copy = $(subst $(LOCAL_SRC_DIR)/gpct/,,$(header_raw))
