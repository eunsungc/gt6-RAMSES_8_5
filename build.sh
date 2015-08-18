#!/bin/sh
./configure
vi Makefile
find . -type f -name aclocal.m4 -exec touch {} \;
find . -type f -name Makefile.in -exec touch {} \;
make
