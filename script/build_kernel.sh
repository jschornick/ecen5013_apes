#!/bin/bash

# File    : build_kernel.sh
# Author  : Jeff Schornick
# Version : See GitHub repo jschornick/ecen5013_apes
#
# Build a new kernel and test it by installing it on a fresh virtual machine.

SCRIPT_DIR=$(dirname $0)

KERNEL_VERSION=v4.15
LINUX_DIR=${HOME}/linux-stable
THREADS=8

VM_NAME=apes
INSTALL_SCRIPT=install_kernel.sh

echo Preparing build space for kernel version: ${KERNEL_VERSION}

pushd ${LINUX_DIR} >/dev/null
git checkout ${KERNEL_VERSION}
make defconfig
#make clean >/dev/null

echo Building new kernel with ${THREADS} threads...
make -j ${THREADS} >/dev/null

echo
popd >/dev/null

echo Bringing up a fresh kernel test VM clone...
${SCRIPT_DIR}/clone.sh
echo

echo -n Waiting for clone to be online...
while ! timeout 0.5 ssh ${VM_NAME} exit 2>/dev/null; do echo -n .; sleep 1; done
echo

echo Installing kernel on VM...
rsync ${SCRIPT_DIR}/${INSTALL_SCRIPT} ${VM_NAME}:/tmp/
echo

ssh ${VM_NAME} /tmp/${INSTALL_SCRIPT}

