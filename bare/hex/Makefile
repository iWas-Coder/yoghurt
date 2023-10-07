##################
# === MACROS === #
##################
# Name
NAME_HEX  = hex
NAME_SEED = hex-seed
NAME_HEX0 = hex0
NAME_VM   = vm
NAME_HEX1 = hex1
NAME_HEX2 = hex2
NAME_LED  = led
NAME_M0   = M0

# Pretty Printing Output (PPO)
PPO_MKDIR = MKDIR
PPO_CLEAN = CLEAN
PPO_HEX0  = HEX0
PPO_HEX1  = HEX1
PPO_HEX2  = HEX2
PPO_HEX   = HEX
PPO_CC    = CC
PPO_LD    = LD

# Directories
SRC_DIR   = src
BUILD_DIR = build

# Files
SRCS_HEX := $(wildcard $(SRC_DIR)/*.c)
OBJS_HEX := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS_HEX))
SRC_SEED  = $(SRC_DIR)/$(NAME_SEED).$(NAME_HEX0)
SRC_HEX0  = $(SRC_DIR)/$(NAME_HEX0).$(NAME_HEX0)
SRC_HEX1  = $(SRC_DIR)/$(NAME_HEX1).$(NAME_HEX0)
SRC_HEX2  = $(SRC_DIR)/$(NAME_HEX2).$(NAME_HEX1)
SRC_LED   = $(SRC_DIR)/$(NAME_LED).$(NAME_HEX2)
SRC_M0    = $(SRC_DIR)/$(NAME_M0).$(NAME_HEX2)

# Compilation flags
CC       = gcc
CPPFLAGS =
CFLAGS   = -Wall -Wextra -Werror -std=gnu2x
LDFLAGS  =

# External targets
BIN_VM = ../$(NAME_VM)/$(BUILD_DIR)/$(NAME_VM)

# Targets
BIN      = $(BUILD_DIR) $(BIN_HEX) $(BIN_SEED) $(BIN_HEX0) $(BIN_HEX1) $(BIN_HEX2) $(BIN_LED) $(BIN_M0)
BIN_HEX  = $(BUILD_DIR)/$(NAME_HEX)
BIN_SEED = $(BUILD_DIR)/$(NAME_SEED)
BIN_HEX0 = $(BUILD_DIR)/$(NAME_HEX0)
BIN_HEX1 = $(BUILD_DIR)/$(NAME_HEX1)
BIN_HEX2 = $(BUILD_DIR)/$(NAME_HEX2)
BIN_LED  = $(BUILD_DIR)/$(NAME_LED)
BIN_M0   = $(BUILD_DIR)/$(NAME_M0)


###################
# === TARGETS === #
###################
.PHONY: all check clean

all: $(BIN)

$(BUILD_DIR):
	@printf "  $(PPO_MKDIR)\t$@\n"
	@mkdir $@

$(BIN_HEX): $(OBJS_HEX)
	@printf "  $(PPO_LD)\t$@\n"
	@$(CC) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@printf "  $(PPO_CC)\t$@\n"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_SEED): $(SRC_SEED) $(BIN_HEX)
	@printf "  $(PPO_HEX)\t$@\n"
	@$(BIN_HEX) < $< > $@

$(BIN_HEX0): $(SRC_HEX0) $(BIN_HEX)
	@printf "  $(PPO_HEX)\t$@\n"
	@$(BIN_HEX) < $< > $@

$(BIN_HEX1): $(SRC_HEX1) $(BIN_VM) $(BIN_HEX0)
	@printf "  $(PPO_HEX0)\t$@\n"
	@$(BIN_VM) --rom $(BIN_HEX0) --tape_01 $< --tape_02 $@

$(BIN_HEX2): $(SRC_HEX2) $(BIN_VM) $(BIN_HEX1)
	@printf "  $(PPO_HEX1)\t$@\n"
	@$(BIN_VM) --rom $(BIN_HEX1) --tape_01 $< --tape_02 $@

$(BIN_LED): $(SRC_LED) $(BIN_VM) $(BIN_HEX2)
	@printf "  $(PPO_HEX2)\t$@\n"
	@$(BIN_VM) --rom $(BIN_HEX2) --tape_01 $< --tape_02 $@

$(BIN_M0): $(SRC_M0) $(BIN_VM) $(BIN_HEX2)
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
