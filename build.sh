#!/bin/sh
./configure --with-udt --enable-udt --prefix=$1
vi Makefile
find . -type f -name aclocal.m4 -exec touch {} \;
find . -type f -name Makefile.in -exec touch {} \;
make
make install
