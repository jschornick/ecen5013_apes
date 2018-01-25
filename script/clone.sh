#!/bin/bash

# Clones an minimally configured base image to test the new kernel on

# Base image has a know IP, SSH keys, and VBox guest additions installed (for
# shared folder support)

VBM=vboxmanage

BASE_VM=apes
CLONE_VM=${BASE_VM}-ktest
SNAP_NAME=ktest

if ${VBM} list runningvms | grep -q ^\"${CLONE_VM}\" ; then
    echo -n "Killing running clone ${CLONE_VM} : "
    vboxmanage controlvm ${CLONE_VM} acpipowerbutton
    sleep 2;
    vboxmanage controlvm ${CLONE_VM} poweroff
    sleep 1;
else
    echo Clone not running
fi

if ${VBM} list vms | grep -q ^\"${CLONE_VM}\" ; then
    echo -n "Removing clone ${CLONE_VM} : "
    vboxmanage unregistervm --delete ${CLONE_VM}
else
    echo No clone to remove
fi

if ${VBM} snapshot ${BASE_VM} list | grep -q "Name: ${SNAP_NAME}" ; then
    #vboxmanage snapshot apes delete base
    echo Using existing snapshot
else
    ${VBM} snapshot ${BASE_VM} take ${SNAP_NAME}
fi

echo -n "Cloning ${BASE_VM} to ${CLONE_VM} : "
vboxmanage clonevm ${BASE_VM} --snapshot ${SNAP_NAME} --options link --name ${CLONE_VM} --register

echo "Starting VM..."

vboxmanage startvm ${CLONE_VM} --type headless
