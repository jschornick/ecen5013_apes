# Makefile for APES tests

THIRD_PARTY=../../../3rd-party
CMOCKA_LIB=$(THIRD_PARTY)/cmocka/BUILD/src/libcmocka.a
CMOCKA_INCLUDE+=-I$(THIRD_PARTY)/cmocka/include

PROJECT_DIR=..
PROJECT_INCLUDE=-I$(PROJECT_DIR)
PROJECT_OBJS=$(PROJECT_DIR)/BUILD/linked_list.o

CFLAGS=-Wall -Werror -g

TESTS=$(patsubst %.c, %.run, $(wildcard test_*.c))

runall: $(TESTS)
	@echo
	-@for x in $^; do echo $$x; ./$$x; echo; done

test_%.run: test_%.c $(CMOCKA_LIB) $(PROJECT_OBJS)
	gcc $(CFLAGS) $(CMOCKA_INCLUDE) $(PROJECT_INCLUDE) $^ -o $@

$(CMOCKA_LIB):
	cd $(THIRD_PARTY); make cmocka

.PHONY: clean
clean:
	rm -rf *.run
