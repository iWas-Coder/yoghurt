##################
# === MACROS === #
##################
# Name
NAME = vm

# Pretty Printing Output (PPO)
PPO_MKDIR = MKDIR
PPO_CLEAN = CLEAN
PPO_CC    = CC
PPO_LD    = LD

# Directories
SRC_DIR   = src
HDR_DIR   = include
BUILD_DIR = build

# Files
SRCS := $(wildcard $(SRC_DIR)/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Compilation flags
CC       = gcc
CPPFLAGS = -I $(HDR_DIR) -DVM32=true -Dtty_lib=true
CFLAGS   = -Wall -Wextra -Werror -std=gnu2x
LDFLAGS  =

# Targets
BIN    = $(BUILD_DIR) $(BIN_VM)
BIN_VM = $(BUILD_DIR)/$(NAME)


###################
# === TARGETS === #
###################
.PHONY: all clean

all: $(BIN)

$(BUILD_DIR):
	@printf "  $(PPO_MKDIR)\t$(BUILD_DIR)\n"
	@mkdir $(BUILD_DIR)

$(BIN_VM): $(OBJS)
	@printf "  $(PPO_LD)\t$@\n"
	@$(CC) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@printf "  $(PPO_CC)\t$@\n"
	@$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	@if [ -d $(BUILD_DIR) ]; then                    \
		printf "  $(PPO_CLEAN)\t$(BUILD_DIR)\n"; \
		rm -r $(BUILD_DIR);                      \
	fi
