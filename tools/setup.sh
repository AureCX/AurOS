#!/bin/bash

# Script to execute in the root folder of the repo

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

apt update -y

apt install -y git gcc gdb binutils make qemu-system-x86 gnu-efi ovmf

if [ ! -f ./OVMF_VARS.fd ]; then
    echo "\"OVMF_VARS.fd\" file not found, copying..."
    if cp /usr/share/OVMF/OVMF_VARS_4M.fd ./OVMF_VARS.fd; then
        echo "File copied successfully."
        if chown "$SUDO_USER:$SUDO_USER" ./OVMF_VARS.fd; then
            echo "File permissions changed successfully."
        else
            echo "Failed to changed the copied file permissions."
            exit 1
        fi
    else
        echo "Failed to copy OVMF_VARS.fd."
        exit 1
    fi
fi