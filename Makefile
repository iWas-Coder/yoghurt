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

MAKE_TGTS = vm hex asm
CLEAN_TGTS = clean-vm clean-hex clean-asm


###################
# === TARGETS === #
###################
.PHONY: $(MAKE_TGTS) $(CLEAN_TGTS)

all: $(MAKE_TGTS)

vm:
	@cd $(VM_DIR) && make

hex:
	@cd $(HEX_DIR) && make && make check

asm:
	@cd $(ASM_DIR) && make && make check

clean: $(CLEAN_TGTS)

clean-vm:
	@cd $(VM_DIR) && make clean

clean-hex:
	@cd $(HEX_DIR) && make clean

clean-asm:
	@cd $(ASM_DIR) && make clean
