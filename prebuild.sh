#!/bin/bash
set -e
mkdir -p build
cd build
echo "======= ARM Toolchain"
if [ ! -d "gcc-arm-none-eabi-6_2-2016q4" ]; then
    TAR_FILE=gcc-arm-none-eabi-6_2-2016q4-20161216-linux.tar.bz2
    echo "------- Download and unpack"
    sudo apt-get -y install lib32ncurses5
    if ! [ -f $TAR_FILE ]; then
        wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/6-2016q4/$TAR_FILE
    fi
    tar xjf $TAR_FILE
else
    echo "------- Already exists"
fi
echo "------- Version check"
gcc-arm-none-eabi-6_2-2016q4/bin/arm-none-eabi-gcc --version
echo "======= RIOT OS"
if [ ! -d "RIOT" ]; then
    echo "------- Clone"
    git clone --depth=1 https://github.com/RIOT-OS/RIOT.git RIOT
else
    echo "------- Already exists"
fi