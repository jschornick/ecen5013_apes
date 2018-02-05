# File        : hw2/p3/makefile
# Description : Makefile for APES Homework 2, Problem 3

MAKEFLAGS += --no-builtin-rules

CC         = gcc

#INCLUDES   = -I$(INC_DIR)

CPP_FLAGS  = $(INCLUDES)
CPP_FLAGS  += -D_GNU_SOURCE

C_FLAGS    = $(CPP_FLAGS)
C_FLAGS    += -std=c99
C_FLAGS    += -O2
C_FLAGS    += -Wall -Werror
#C_FLAGS    += -g3 -gstrict-dwarf

SRC_DIR = .
BUILD_DIR = BUILD

C_SOURCES = $(NAME).c
OBJECTS   = $(addprefix $(BUILD_DIR)/, $(C_SOURCES:.c=.o))

NAME = hw2_p3
BINARY = $(NAME).elf

# Convenience targets
%.o: $(BUILD_DIR)/%.o ;
%.i: $(BUILD_DIR)/%.i ;

.PRECIOUS: $(BUILD_DIR)/%.i $(BUILD_DIR)/%.o

.PHONY: all
all: $(NAME).elf

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.i: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(C_FLAGS) -E $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@

%.elf: $(BUILD_DIR)/%.o $(BUILD_DIR)
	$(CC) $< $(LD_FLAGS) -o $@

.PHONY: syscall
syscall:
	make -C ~/linux-stable apes

.PHONY: ktest
ktest: syscall $(BINARY)
	-build_kernel.sh hw2_p3
	sleep 5
	rsync -va $(BINARY) apes:

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) *.elf *.o
