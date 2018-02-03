#!/bin/bash

# Install a previously built kernel mounted via VirtualBox shared folder

mount -t vboxsf linux-stable /mnt/

make -C /mnt/ modules_install
make -C /mnt/ install

ls -l /boot/

reboot

