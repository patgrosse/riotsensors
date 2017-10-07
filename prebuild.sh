#!/bin/bash
set -e
cd lib
GITLAB_PREFIX="${GITLAB_PREFIX:-https://gitlab.com/}"
if [ ! -z ${GITLAB_USE_SSH+x} ]; then
    GITLAB_PREFIX="git@gitlab.com:"
fi
echo "======= libspt"
if [ ! -d "libspt" ]; then
    echo "------- Using git clone"
    git clone --depth 1 ${GITLAB_PREFIX}patgrosse/libspt.git libspt
else
    echo "------- Already exists"
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
    git clone --depth 1 -b riotsensors https://github.com/patgrosse/RIOT.git RIOT
else
    echo "------- Already exists"
fi
echo "======= Googletest"
if [ ! -d "googletest" ]; then
    echo "------- Using git clone"
    git clone --depth 1 https://github.com/google/googletest.git googletest
else
    if [ ! -e "googletest/.git" ]; then
        echo "------- Using git submodule"
        git submodule update --init --depth 1 -- googletest
    else
        echo "------- Already exists"
    fi
fi
echo "======= rapidjson"
if [ ! -d "rapidjson" ]; then
    echo "------- Using git clone"
    git clone --depth 1 https://github.com/miloyip/rapidjson.git rapidjson
else
    if [ ! -e "rapidjson/.git" ]; then
        echo "------- Using git submodule"
        git submodule update --init --depth 1 -- rapidjson
    else
        echo "------- Already exists"
    fi
fi
echo "======= Pistache"
if [ ! -d "pistache" ]; then
    echo "------- Using git clone"
    git clone --depth 1 https://github.com/oktal/pistache.git pistache
else
    if [ ! -e "pistache/.git" ]; then
        echo "------- Using git submodule"
        git submodule update --init --depth 1 -- pistache
    else
        echo "------- Already exists"
    fi
fi
echo "======= microcoap"
if [ ! -d "microcoap" ]; then
    echo "------- Using git clone"
    git clone --depth 1 https://github.com/1248/microcoap.git microcoap
else
    if [ ! -e "microcoap/.git" ]; then
        echo "------- Using git submodule"
        git submodule update --init --depth 1 -- microcoap
    else
        echo "------- Already exists"
    fi
fi
