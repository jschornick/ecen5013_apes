# File        : hw4/p2/makefile
# Description : Makefile for APES Homework 4, Problem 2
# Author      : Jeff Schornick (jesc5667@colorado.edu)
# Version     : Version and history information is available at the GitHub repository:
#               https://github.com/jschornick/ecen5013_apes/hw4

MAKEFLAGS += --no-builtin-rules

CC         = gcc

CPP_FLAGS  = -I$(INC_DIR)
CPP_FLAGS += -pthread

C_FLAGS    = $(CPP_FLAGS)
C_FLAGS   += -std=gnu99
C_FLAGS   += -Wall -Werror
C_FLAGS   += -O0
C_FLAGS   += -g3 -gstrict-dwarf

# rt for posix timers
LD_FLAGS   = -pthread -lrt

SRC_DIR = src
INC_DIR = inc
BUILD_DIR = BUILD

NAMES = pipes sockets shmem queues

EXES = $(addsuffix .elf, $(addprefix hw4p2_, $(NAMES)))
COMMON_SRCS = messages.c
COMMON_OBJS = $(addprefix $(BUILD_DIR)/, $(COMMON_SRCS:.c=.o))

# Convenience targets
%.o: $(BUILD_DIR)/%.o ;
%.i: $(BUILD_DIR)/%.i ;

.PRECIOUS: $(BUILD_DIR)/%.i $(BUILD_DIR)/%.o

.PHONY: default
default: $(EXES)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.i: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CPP_FLAGS) -E $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@

%.elf: $(BUILD_DIR)/%.o $(COMMON_OBJS)
	$(CC) $(LD_FLAGS) $^ -o $@

.PHONY: tests
tests: $(BUILD_DIR)/linked_list.o
	@echo Building and running unit tests...
	cd tests && make

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) *.elf
