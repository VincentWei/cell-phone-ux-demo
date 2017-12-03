#!/bin/bash

LIST="dirent gtk+ libxml minigui-misc pthread "

###################################################

INCLUDE=$PREFIX/include
LIB=$PREFIX/lib
BIN=$PREFIX/bin

mkdir $INCLUDE -p
mkdir $LIB -p
mkdir $BIN -p

for i in $LIST; do
    if [ -d $_ARCH_ROOT/$i/include/ ]; then
        cp $_ARCH_ROOT/$i/include/* $INCLUDE -a
    fi

    if [ -d $_ARCH_ROOT/$i/lib/ ]; then
        cp $_ARCH_ROOT/$i/lib/* $LIB -a
    fi

    if [ -d $_ARCH_ROOT/$i/bin/ ]; then
        cp $_ARCH_ROOT/$i/bin/* $BIN -a
    fi
done

