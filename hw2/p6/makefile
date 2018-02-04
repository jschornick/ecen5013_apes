# File        : hw2/makefile
# Description : Makefile for APES Homework 2, Problem 6

MAKEFLAGS += --no-builtin-rules

CC         = gcc

#INCLUDES   = -I$(INC_DIR)

CPP_FLAGS  = $(INCLUDES)

C_FLAGS    = $(CPP_FLAGS)
C_FLAGS    += -std=c99
C_FLAGS    += -O2
C_FLAGS    += -Wall -Werror
#C_FLAGS    += -g3 -gstrict-dwarf

SRC_DIR = .
BUILD_DIR = BUILD

# Convenience targets
%.o: $(BUILD_DIR)/%.o ;
%.i: $(BUILD_DIR)/%.i ;

.PRECIOUS: $(BUILD_DIR)/%.i $(BUILD_DIR)/%.o

.PHONY: all
all: test

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.i: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(C_FLAGS) -E $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@

$(NAME).elf: $(BUILD_DIR)/%.o $(BUILD_DIR)/%linked_list.o $(BUILD_DIR)
	$(CC) $< $(LD_FLAGS) -o $@

test.elf: $(BUILD_DIR)/linked_list.o $(BUILD_DIR)/test.o
	$(CC) $^ $(LD_FLAGS) -o $@

.PHONY: test
test: test.elf
	./test.elf

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) *.elf *.o *.out
