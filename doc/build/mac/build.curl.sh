#!/bin/sh
export CC=gcc-4.0

export CFLAGS="-O -g -isysroot /Developer/SDKs/MacOSX10.4u.sdk -mmacosx-version-min=10.4 -arch i386 -arch ppc"
./configure --prefix=/usr/local/encap/curl-7.19.6 --with-ssl=/usr --without-ca-bundle --disable-dependency-tracking

cp include/curl/curlbuild.h include/curl/curlbuild32.h

make distclean

export CFLAGS="-O -g -isysroot /Developer/SDKs/MacOSX10.4u.sdk -mmacosx-version-min=10.4 -arch x86_64"
./configure --prefix=/usr/local/encap/curl-7.19.6 --with-ssl=/usr --without-ca-bundle --disable-dependency-tracking

cp include/curl/curlbuild.h include/curl/curlbuild64.h

make distclean

export CFLAGS="-O -g -isysroot /Developer/SDKs/MacOSX10.4u.sdk -mmacosx-version-min=10.4 -arch i386 -arch ppc -arch x86_64"
./configure --prefix=/usr/local/encap/curl-7.19.6 --with-ssl=/usr --without-ca-bundle --disable-dependency-tracking

cat > include/curl/curlbuild.h <<EOF
#ifdef __LP64__
#include "curlbuild64.h"
#else
#include "curlbuild32.h"
#endif 
EOF

make
