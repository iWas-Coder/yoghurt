##################
# === MACROS === #
##################
# Name
NAME_VM     = vm
NAME_M0     = M0
NAME_HEX    = hex
NAME_CAT    = cat
NAME_HEX2   = hex2
NAME_LISP   = lisp
NAME_FORTH  = forth
NAME_CC_x86 = cc_x86

# Pretty Printing Output (PPO)
PPO_MKDIR = MKDIR
PPO_CLEAN = CLEAN
PPO_HEX2  = HEX2
PPO_CAT   = CAT
PPO_M0    = M0

# Directories
SRC_DIR   = src
BUILD_DIR = build

# Files
SRC_CAT = $(SRC_DIR)/defs.M0 $(SRC_DIR)/$(NAME_CAT).S
SRC_LISP = $(SRC_DIR)/defs.M0 $(SRC_DIR)/$(NAME_LISP).S
SRC_FORTH = $(SRC_DIR)/defs.M0 $(SRC_DIR)/$(NAME_FORTH).S
SRC_CC_x86 = $(SRC_DIR)/defs.M0 $(SRC_DIR)/$(NAME_CC_x86).S

# External targets
BIN_VM = ../$(NAME_VM)/$(BUILD_DIR)/$(NAME_VM)
BIN_M0 = ../$(NAME_HEX)/$(BUILD_DIR)/$(NAME_M0)
BIN_HEX2 = ../$(NAME_HEX)/$(BUILD_DIR)/$(NAME_HEX2)

# Targets
BIN          = $(BUILD_DIR) $(BIN_CAT) $(BIN_LISP) $(BIN_FORTH) $(BIN_CC_x86)
BIN_CAT      = $(BUILD_DIR)/$(NAME_CAT)
BIN_CAT_TMP1 = $(BUILD_DIR)/$(NAME_CAT).1.tmp
BIN_CAT_TMP2 = $(BUILD_DIR)/$(NAME_CAT).2.tmp
BIN_LISP      = $(BUILD_DIR)/$(NAME_LISP)
BIN_LISP_TMP1 = $(BUILD_DIR)/$(NAME_LISP).1.tmp
BIN_LISP_TMP2 = $(BUILD_DIR)/$(NAME_LISP).2.tmp
BIN_FORTH      = $(BUILD_DIR)/$(NAME_FORTH)
BIN_FORTH_TMP1 = $(BUILD_DIR)/$(NAME_FORTH).1.tmp
BIN_FORTH_TMP2 = $(BUILD_DIR)/$(NAME_FORTH).2.tmp
BIN_CC_x86   = $(BUILD_DIR)/$(NAME_CC_x86)
BIN_CC_x86_TMP1 = $(BUILD_DIR)/$(NAME_CC_x86).1.tmp
BIN_CC_x86_TMP2 = $(BUILD_DIR)/$(NAME_CC_x86).2.tmp


###################
# === TARGETS === #
###################
.PHONY: all check clean

all: $(BIN)

$(BUILD_DIR):
	@printf "  $(PPO_MKDIR)\t$(BUILD_DIR)\n"
	@mkdir $(BUILD_DIR)

$(BIN_CAT_TMP1): $(SRC_CAT)
	@printf "  $(PPO_CAT)\t$@\n"
	@cat $^ > $@

$(BIN_CAT_TMP2): $(BIN_CAT_TMP1) $(BIN_VM) $(BIN_M0)
	@printf "  $(PPO_M0)\t$@\n"
	@$(BIN_VM) --rom $(BIN_M0) --tape_01 $< --tape_02 $@ --memory 48K

$(BIN_CAT): $(BIN_CAT_TMP2) $(BIN_VM) $(BIN_HEX2)
	@printf "  $(PPO_HEX2)\t$@\n"
	@$(BIN_VM) --rom $(BIN_HEX2) --tape_01 $< --tape_02 $@ --memory 48K

$(BIN_LISP_TMP1): $(SRC_LISP)
	@printf "  $(PPO_CAT)\t$@\n"
	@cat $^ > $@

$(BIN_LISP_TMP2): $(BIN_LISP_TMP1) $(BIN_VM) $(BIN_M0)
	@printf "  $(PPO_M0)\t$@\n"
	@$(BIN_VM) --rom $(BIN_M0) --tape_01 $< --tape_02 $@ --memory 256K

$(BIN_LISP): $(BIN_LISP_TMP2) $(BIN_VM) $(BIN_HEX2)
	@printf "  $(PPO_HEX2)\t$@\n"
	@$(BIN_VM) --rom $(BIN_HEX2) --tape_01 $< --tape_02 $@ --memory 48K

$(BIN_FORTH_TMP1): $(SRC_FORTH)
	@printf "  $(PPO_CAT)\t$@\n"
	@cat $^ > $@

$(BIN_FORTH_TMP2): $(BIN_FORTH_TMP1) $(BIN_VM) $(BIN_M0)
	@printf "  $(PPO_M0)\t$@\n"
	@$(BIN_VM) --rom $(BIN_M0) --tape_01 $< --tape_02 $@ --memory 128K

$(BIN_FORTH): $(BIN_FORTH_TMP2) $(BIN_VM) $(BIN_HEX2)
	@printf "  $(PPO_HEX2)\t$@\n"
	@$(BIN_VM) --rom $(BIN_HEX2) --tape_01 $< --tape_02 $@ --memory 48K

$(BIN_CC_x86_TMP1): $(SRC_CC_x86)
	@printf "  $(PPO_CAT)\t$@\n"
	@cat $^ > $@

$(BIN_CC_x86_TMP2): $(BIN_CC_x86_TMP1) $(BIN_VM) $(BIN_M0)
	@printf "  $(PPO_M0)\t$@\n"
	@$(BIN_VM) --rom $(BIN_M0) --tape_01 $< --tape_02 $@ --memory 256K

$(BIN_CC_x86): $(BIN_CC_x86_TMP2) $(BIN_VM) $(BIN_HEX2)
	@printf "  $(PPO_HEX2)\t$@\n"
	@$(BIN_VM) --rom $(BIN_HEX2) --tape_01 $< --tape_02 $@ --memory 48K

check:
	@printf "\n==== CHECKSUM CHECK starts ====\n"
	@sha256sum -c checksums.sha256
	@printf "====  CHECKSUM CHECK ends  ====\n\n"

clean:
	@if [ -d $(BUILD_DIR) ]; then                    \
		printf "  $(PPO_CLEAN)\t$(BUILD_DIR)\n"; \
		rm -r $(BUILD_DIR);                      \
	fi
