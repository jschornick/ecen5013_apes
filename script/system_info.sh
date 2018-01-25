#!/bin/bash

# File    : system_info.sh
# Author  : Jeff Schornick
# Version : See GitHub repo jschornick/ecen5013_apes
#
# A simple script to report information about the current system.
#
# Tested on Ubuntu 16.04 and 17.10, Debian 8, and Arch/Antergos

# Allow output to a file, or default to standard out
# (NB: Normally this would be handled using file redirection, not explicitly in the script...)
outfile=${1:-/dev/stdout}

# Use a single associate array to store all system information
declare -A info

# Parse a key from the /etc/os-release file
function read_release {
    awk -F\" /^$1=/'{print $2}' /etc/os-release
}

# Parse the version of GCC used to build the kernel.
# This is a bit tricky since it is the second of two strings contained within a
# matching set of parentheses.
function read_kernel_gcc {
  gcc_string=$(awk -F "[()]" '{print $4 "(" $5 ")"}' /proc/version)
  echo $gcc_string | sed 's/^gcc version \(.*\)/\1/'
}

# Collect all the information, but don't output
info[Hostname]=$(uname -n)
info[OS_Type]=$(uname -o)
info[Distribtuion]=$(read_release NAME)
info[OS_Release]=$(read_release VERSION)
info[Kernel_Version]=$(uname -r)
info[Kernel_BuildInfo]=$(uname -v)
info[Architecture]=$(uname -m)
info[Root_FS_free]=$(df -h / | awk 'NR>1 {print $4}' | tail -1)
info[Current_User]=$(id | awk -F"[= ]" '{print $2}')
info[Kernel_GCC]=$(read_kernel_gcc)

# Print a header and each key/value pair collected
(echo System Information:;
 echo -------------------) >$outfile

(for key in "${!info[@]}"; do
    echo $key : ${info[$key]}
done) | sort >>$outfile

