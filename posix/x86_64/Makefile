##################
# === MACROS === #
##################
# Name
NAME_SEED   = hex0-seed
NAME_HEX0   = hex0
NAME_HEX1   = hex1
NAME_HEX2   = hex2
NAME_HEX2_0 = hex2-0
NAME_CATM   = catm
NAME_ELF    = ELF
NAME_M0     = M0

# Pretty Printing Output (PPO)
PPO_MKDIR = MKDIR
PPO_CLEAN = CLEAN
PPO_SEED  = SEED
PPO_HEX0  = HEX0
PPO_HEX1  = HEX1
PPO_HEX2  = HEX2
PPO_CATM  = CATM
PPO_M0    = M0
PPO_CC    = CC

SEED = seed/$(NAME_SEED)

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

# Files
SRC_HEX0   = $(SRC_DIR)/$(NAME_HEX0).$(NAME_HEX0)
SRC_HEX1   = $(SRC_DIR)/$(NAME_HEX1).$(NAME_HEX0)
SRC_HEX2_0 = $(SRC_DIR)/$(NAME_HEX2).$(NAME_HEX1)
SRC_CATM   = $(SRC_DIR)/$(NAME_CATM).$(NAME_HEX2)
SRC_ELF    = $(SRC_DIR)/$(NAME_ELF).$(NAME_HEX2)
SRC_M0    := $(SRC_ELF) $(SRC_DIR)/$(NAME_M0).$(NAME_HEX2)

# Targets
BIN        = $(BIN_DIR) $(BIN_HEX0) $(BIN_HEX1) $(BIN_HEX2_0) $(BIN_CATM) $(OBJ_M0) $(BIN_M0)
BIN_HEX0   = $(BIN_DIR)/$(NAME_HEX0)
BIN_HEX1   = $(BIN_DIR)/$(NAME_HEX1)
BIN_HEX2_0 = $(BIN_DIR)/$(NAME_HEX2_0)
BIN_CATM   = $(BIN_DIR)/$(NAME_CATM)
OBJ_M0     = $(BUILD_DIR)/$(NAME_M0).$(NAME_HEX2)
BIN_M0     = $(BIN_DIR)/$(NAME_M0)


###################
# === TARGETS === #
###################
.PHONY: all check clean

all: $(BIN)

$(BUILD_DIR):
	@printf "  $(PPO_MKDIR)\t$@\n"
	@mkdir $@

$(BIN_DIR): $(BUILD_DIR)
	@printf "  $(PPO_MKDIR)\t$@\n"
	@mkdir $@

# Phase 0: seed -> hex0
$(BIN_HEX0): $(SRC_HEX0) $(SEED)
	@printf "  $(PPO_SEED)\t$@\n"
	@$(SEED) $< $@

# Phase 1: hex0 -> hex1
$(BIN_HEX1): $(SRC_HEX1) $(BIN_HEX0)
	@printf "  $(PPO_HEX0)\t$@\n"
	@$(BIN_HEX0) $< $@

# Phase 2: hex1 -> hex2-0
$(BIN_HEX2_0): $(SRC_HEX2_0) $(BIN_HEX1)
	@printf "  $(PPO_HEX1)\t$@\n"
	@$(BIN_HEX1) $< $@

# Phase 2b: hex2-0 -> catm
$(BIN_CATM): $(SRC_CATM) $(BIN_HEX2_0)
	@printf "  $(PPO_HEX2)\t$@\n"
	@$(BIN_HEX2_0) $< $@

# Phase 3: (catm + hex2-0) -> M0
$(OBJ_M0): $(SRC_M0) $(BIN_CATM)
	@printf "  $(PPO_CATM)\t$@\n"
	@$(BIN_CATM) $@ $(SRC_M0)
$(BIN_M0): $(OBJ_M0) $(BIN_HEX2_0)
	@printf "  $(PPO_HEX2)\t$@\n"
	@$(BIN_HEX2_0) $< $@

# Phase 4: (M0 + catm + hex2-0) -> cc

check:
	@printf "\n==== CHECKSUM CHECK starts ====\n"
	@sha256sum -c checksums.sha256
	@printf "====  CHECKSUM CHECK ends  ====\n\n"

clean:
	@if [ -d $(BUILD_DIR) ]; then                    \
		printf "  $(PPO_CLEAN)\t$(BUILD_DIR)\n"; \
		rm -r $(BUILD_DIR);                      \
	fi
