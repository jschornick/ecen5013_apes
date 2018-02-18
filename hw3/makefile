# File        : hw3/makefile
# Description : Makefile for APES Homework 3
# Author      : Jeff Schornick (jesc5667@colorado.edu)
# Version     : Version and history information is available at the GitHub repository:
#               https://github.com/jschornick/ecen5013_apes/hw3

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

NAME   = hw3
EXE    = $(NAME).elf

C_SRCS = $(NAME).c linked_list.c logging.c threads.c
OBJS = $(addprefix $(BUILD_DIR)/, $(C_SRCS:.c=.o))

# Convenience targets
%.o: $(BUILD_DIR)/%.o ;
%.i: $(BUILD_DIR)/%.i ;

.PRECIOUS: $(BUILD_DIR)/%.i $(BUILD_DIR)/%.o

.PHONY: default
default: $(EXE)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.i: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CPP_FLAGS) -E $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(C_FLAGS) -c $< -o $@

$(EXE): $(OBJS)
	$(CC) $(LD_FLAGS) $(OBJS) -o $@

.PHONY: tests
tests: $(BUILD_DIR)/linked_list.o
	@echo Building and running unit tests...
	cd tests && make

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) $(BINARY)
