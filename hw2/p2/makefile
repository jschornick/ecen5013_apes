# File        : hw2/makefile
# Description : Makefile for APES Homework 2

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

C_SOURCES = $(NAME).c
OBJECTS   = $(addprefix $(BUILD_DIR)/, $(C_SOURCES:.c=.o))

NAME = hw2_p2
BINARY = $(NAME).elf

READ_STDIN=1
ifdef READ_STDIN
	CPP_FLAGS  += -DREAD_STDIN
	PERF_CMD=echo input_string | perf stat ./$(BINARY)
	STRACE_CMD=echo input_string | strace ./$(BINARY)
	LTRACE_CMD=echo input_string | ltrace ./$(BINARY)
else
	PERF_CMD=perf stat ./$(BINARY) input_param
	STRACE_CMD=strace ./$(BINARY) input_param
	LTRACE_CMD=ltrace ./$(BINARY) input_param
endif

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

.PHONY: profile
profile: $(BINARY)
	$(PERF_CMD) 2>perf.out
	$(STRACE_CMD) 2>strace.out
	$(LTRACE_CMD) 2>ltrace.out

.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR) *.elf *.o *.out
