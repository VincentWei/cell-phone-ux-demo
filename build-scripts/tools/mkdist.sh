#!/bin/sh

echo "tar file names $1.tar.bz2"
tar cfjv $1.tar.bz2 $1  --exclude=core-dump
