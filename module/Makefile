include common.mk

SUBDIRS += test

include module/arch/$(ARCH)/Makefile

.PHONY: all $(SUBDIRS)

all: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -s -f module/$@/Makefile
