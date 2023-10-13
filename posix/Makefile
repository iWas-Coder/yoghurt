##################
# === MACROS === #
##################
# Makefile flags for sublevel Makefile files
ifndef VERBOSE
	MAKEFLAGS += --no-print-directory
endif

HOST_ARCH = $(shell uname -m)

X86_64_DIR = x86_64

MAKE_TGTS = $(HOST_ARCH)
CLEAN_TGTS = clean-$(HOST_ARCH)


###################
# === TARGETS === #
###################
.PHONY: $(MAKE_TGTS) $(CLEAN_TGTS)

all: $(MAKE_TGTS)

$(X86_64_DIR):
	@cd $@ && make && make check

clean: $(CLEAN_TGTS)

clean-$(X86_64_DIR):
	@cd $(X86_64_DIR) && make clean
