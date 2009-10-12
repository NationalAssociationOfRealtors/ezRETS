#!/bin/sh

export CXXFLAGS="-mmacosx-version-min=10.4 -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc -arch x86_64"

export CC=gcc-4.0
export CXX=g++-4.0

./configure --prefix=/usr/local/encap/antlr-2.7.7 --with-javac=javac
make
