#!/bin/sh
DEPS="$(pwd)/deps"

cd ./third_party/zlib
echo "./configure --prefix=${DEPS} --static --64"
./configure --prefix=${DEPS}
make;make install
cd ../..

cd ./third_party/bzip2
echo "make install PREFIX=${DEPS}"
make install PREFIX=${DEPS}
cd ../..

cd ./third_party/xz
echo "./configure --prefix=${DEPS} --disable-lzmainfo --disable-lzma-links --disable-scripts --disable-doc --disable-shared --enable-static --disable-xz"
./configure --prefix=${DEPS} --disable-lzmainfo --disable-lzmadec --disable-lzma-links --disable-scripts --disable-doc --disable-shared --enable-static --disable-xz --disable-xzdec
make;make install
cd ../..

cd ./third_party/libarchive
echo "CPPFLAGS="-I../../deps/include" ./configure --prefix=${DEPS} --disable-bsdtar --disable-bsdcpio --disable-shared --enable-static --without-xml2 --without-expat --without-lzo2 --without-iconv --without-nettle --without-openssl"
CPPFLAGS="-I../../deps/include" ./configure --prefix=${DEPS} --disable-bsdtar --disable-bsdcpio --disable-shared --enable-static --without-xml2 --without-expat --without-lzo2 --without-iconv --without-nettle --without-openssl
make;make install
cd ../..
