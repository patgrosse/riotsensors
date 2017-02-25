#!/bin/bash
set -e
cd lib
echo "======= libspt"
if [ ! -d "libspt" ]; then
    echo "------- Using git clone"
    git clone --depth=1 git@gitlab.com:patgrosse/libspt.git libspt
else
    if [ ! -e "libspt/.git" ]; then
        echo "------- Using git submodule"
        git submodule update --init -- libspt
    else
        echo "------- Already exists"
    fi
fi
echo "======= ARM Toolchain"
if ! type "arm-none-eabi-gcc" &> /dev/null; then
    echo "------- Command not found in path"
    if [ ! -d "gcc-arm-none-eabi-6_2-2016q4" ]; then
        TAR_FILE=gcc-arm-none-eabi-6_2-2016q4-20161216-linux.tar.bz2
        echo "------- Download and unpack"
        if ! [ -f ${TAR_FILE} ]; then
            wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/6-2016q4/${TAR_FILE}
        fi
        tar xjf ${TAR_FILE}
    else
        echo "------- Already exists"
    fi
    echo "------- Version check"
    gcc-arm-none-eabi-6_2-2016q4/bin/arm-none-eabi-gcc --version
else
    echo "------- Command found in path"
    echo "------- Version check"
    arm-none-eabi-gcc --version
fi
echo "======= RIOT OS"
if [ ! -d "RIOT" ]; then
    echo "------- Using git clone"
    git clone --depth=1 https://github.com/RIOT-OS/RIOT.git RIOT
else
    if [ ! -e "RIOT/.git" ]; then
        echo "------- Using git submodule"
        git submodule update --init -- RIOT
    else
        echo "------- Already exists"
    fi
fi
echo "======= Googletest"
if [ ! -d "googletest" ]; then
    echo "------- Using git clone"
    git clone --depth=1 https://github.com/google/googletest.git googletest
else
    if [ ! -e "googletest/.git" ]; then
        echo "------- Using git submodule"
        git submodule update --init -- googletest
    else
        echo "------- Already exists"
    fi
fi
echo "======= SGLIB"
if [ ! -d "sglib" ]; then
    echo "------- Using git clone"
    git clone --depth=1 https://github.com/stefanct/sglib.git sglib
else
    if [ ! -e "sglib/.git" ]; then
        echo "------- Using git submodule"
        git submodule update --init -- sglib
    else
        echo "------- Already exists"
    fi
fi
echo "======= Pistache"
if [ ! -d "pistache" ]; then
    echo "------- Using git clone"
    git clone --depth=1 https://github.com/oktal/pistache.git pistache
else
    if [ ! -e "pistache/.git" ]; then
        echo "------- Using git submodule"
        git submodule update --init -- pistache
    else
        echo "------- Already exists"
    fi
fi