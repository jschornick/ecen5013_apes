# File        : hw4/p3/Makefile
# Description : Makefile for APES Homework 4, Problem 3
# 
# Build a linux kernel module out of tree

LINUX_DIR = ~/linux-stable

obj-m += hw4p3.o

all:
	make -C $(LINUX_DIR) M=$(shell pwd) modules

.PHONY: ktest
ktest:
	-../../script/build_kernel.sh hw4
	sleep 5
	rsync -va $(BINARY) apes:

clean:
	make -C $(LINUX_DIR) M=$(shell pwd) clean
