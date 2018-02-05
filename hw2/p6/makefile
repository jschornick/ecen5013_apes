# File        : hw2/makefile
# Description : Makefile for APES Homework 2, Problem 6
# Author      : Jeff Schornick (jesc5667@colorado.edu)
# Version     : Version and history information is available at the GitHub repository:
#               https://github.com/jschornick/ecen5013_apes/hw2

MAKEFLAGS += --no-builtin-rules

CC         = gcc

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
all: tests

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.i: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(C_FLAGS) -E $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@

.PHONY: tests
tests: $(BUILD_DIR)/linked_list.o
	@echo Building and running unit tests...
	cd tests && make

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) *.elf *.o *.out
