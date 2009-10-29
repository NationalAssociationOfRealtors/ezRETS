#!/bin/sh

export CFLAGS="-mmacosx-version-min=10.4 -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc -arch x86_64"
export CXXFLAGS="-mmacosx-version-min=10.4 -isysroot /Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc -arch x86_64"

export CC=gcc-4.0
export CXX=g++-4.0

./configure --prefix=/usr/local/encap/expat-2.0.1 --disable-dependency-tracking
make
