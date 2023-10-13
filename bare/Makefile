##################
# === MACROS === #
##################
# Makefile flags for sublevel Makefile files
ifndef VERBOSE
	MAKEFLAGS += --no-print-directory
endif

VM_DIR  = vm
HEX_DIR = hex
ASM_DIR = asm

MAKE_TGTS  = $(VM_DIR) $(HEX_DIR) $(ASM_DIR)
CLEAN_TGTS = clean-$(VM_DIR) clean-$(HEX_DIR) clean-$(ASM_DIR)


###################
# === TARGETS === #
###################
.PHONY: $(MAKE_TGTS) $(CLEAN_TGTS)

all: $(MAKE_TGTS)

$(VM_DIR):
	@cd $@ && make

$(HEX_DIR):
	@cd $@ && make && make check

$(ASM_DIR):
	@cd $@ && make && make check

clean: $(CLEAN_TGTS)

clean-$(VM_DIR):
	@cd $(VM_DIR) && make clean

clean-$(HEX_DIR):
	@cd $(HEX_DIR) && make clean

clean-$(ASM_DIR):
	@cd $(ASM_DIR) && make clean
