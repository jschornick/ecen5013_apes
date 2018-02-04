# File        : hw2/p5/Makefile
# Description : Makefile for APES Homework 2, Problem 5
# 
# Build a linux kernel module out of tree

LINUX_DIR = ~/linux-stable

obj-m += apes.o

all:
	make -C $(LINUX_DIR) M=$(shell pwd) modules

clean:
	make -C $(LINUX_DIR) M=$(shell pwd) clean
