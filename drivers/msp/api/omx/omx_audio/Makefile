#===============================================================================
# export variable
#===============================================================================
ifeq ($(CFG_HI_EXPORT_FLAG),)
SDK_DIR := $(shell cd $(CURDIR)/../../../../.. && /bin/pwd)

include $(SDK_DIR)/base.mak
endif

#===============================================================================
objects := omx_adec omx_aenc
objects_clean := $(addsuffix _clean,$(objects))
#===============================================================================
depends := common
depends_clean := $(addsuffix _clean,$(depends))
#===============================================================================
.PHONY: all clean
#===============================================================================
all: $(objects)
clean: $(objects_clean)
#===============================================================================
.PHONY: $(objects) $(objects_clean)
#===============================================================================
$(objects): $(depends)
	$(AT)make -C $@

$(objects_clean): $(depends_clean)
	$(AT)make -C $(patsubst %_clean,%,$@) clean
#===============================================================================
.PHONY: $(depends) $(depends_clean)
#===============================================================================
$(depends):
	$(AT)make -C $@

$(depends_clean):
	$(AT)make -C $(patsubst %_clean,%,$@) clean
#===============================================================================
