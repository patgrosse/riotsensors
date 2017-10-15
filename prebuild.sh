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
    git clone --depth=1 ${GITLAB_PREFIX}patgrosse/libspt.git libspt
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
    git clone --depth=1 -b riotsensors https://github.com/patgrosse/RIOT.git RIOT
else
    echo "------- Already exists"
fi
echo "======= Googletest"
if [ ! -d "googletest" ]; then
    echo "------- Using GitHub release tarball"
    wget -O ".curlib.tar.gz" https://github.com/google/googletest/archive/release-1.8.0.tar.gz
    tar -xf ".curlib.tar.gz"
    rm ".curlib.tar.gz"
    mv googletest-release-1.8.0 googletest
else
    echo "------- Already exists"
fi
echo "======= rapidjson"
if [ ! -d "rapidjson" ]; then
    echo "------- Using GitHub release tarball"
    wget -O ".curlib.tar.gz" https://github.com/Tencent/rapidjson/archive/f05edc9296507a9864d99931e203631c2ffd8d4a.tar.gz
    tar -xf ".curlib.tar.gz"
    rm ".curlib.tar.gz"
    mv rapidjson-f05edc9296507a9864d99931e203631c2ffd8d4a rapidjson
else
    echo "------- Already exists"
fi
echo "======= Pistache"
if [ ! -d "pistache" ]; then
    echo "------- Using GitHub release tarball"
    wget -O ".curlib.tar.gz" https://github.com/oktal/pistache/archive/b66415aaa0dc443b7874acbbb488058b34c2dc01.tar.gz
    tar -xf ".curlib.tar.gz"
    rm ".curlib.tar.gz"
    mv pistache-b66415aaa0dc443b7874acbbb488058b34c2dc01 pistache
else
    echo "------- Already exists"
fi
echo "======= microcoap"
if [ ! -d "microcoap" ]; then
    echo "------- Using GitHub release tarball"
    wget -O ".curlib.tar.gz" https://github.com/1248/microcoap/archive/ef272895925f0d4c563725fe0102966f544a0fdc.tar.gz
    tar -xf ".curlib.tar.gz"
    rm ".curlib.tar.gz"
    mv microcoap-ef272895925f0d4c563725fe0102966f544a0fdc microcoap
else
    echo "------- Already exists"
fi
