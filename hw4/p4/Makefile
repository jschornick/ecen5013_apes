# File        : hw4/p3/Makefile
# Description : Makefile for APES Homework 4, Problem 3
# 
# Build a linux kernel module out of tree

LINUX_DIR = ~/linux-stable

obj-m += hw4p4.o

MODULE = hw4p4.ko

all:
	make -C $(LINUX_DIR) M=$(shell pwd) modules
	rsync -va $(MODULE) apes:

.PHONY: ktest
ktest:
	-../../script/build_kernel.sh hw4

clean:
	make -C $(LINUX_DIR) M=$(shell pwd) clean
