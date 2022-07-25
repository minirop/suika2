#!/bin/sh

PREFIX=`pwd`/libroot

rm -rf tmp libroot
mkdir -p tmp libroot

cd tmp

tar xzf ../../libsrc/zlib-1.2.11.tar.gz
cd zlib-1.2.11
CFLAGS="-O3 -arch arm64 -arch armv7 -isysroot `xcrun -sdk iphoneos --show-sdk-path` -fembed-bitcode -mios-version-min=8.0" ./configure --prefix=$PREFIX --static --archs="-arch arm64 -arch armv7"
make -j4
make install
cd ..

tar xzf ../../libsrc/libpng-1.6.35.tar.gz
cd libpng-1.6.35
./configure --prefix=$PREFIX --host=arm-apple-darwin --disable-shared CPPFLAGS=-I$PREFIX/include CFLAGS="-O3 -arch arm64 -arch armv7 -isysroot `xcrun -sdk iphoneos --show-sdk-path` -fembed-bitcode -mios-version-min=8.0" LDFLAGS="-L$PREFIX/lib -arch arm64 -arch armv7"
make -j4
make install
cd ..

tar xzf ../../libsrc/jpegsrc.v9e.tar.gz
cd jpeg-9e
./configure --prefix=$PREFIX --host=arm-apple-darwin --disable-shared CPPFLAGS=-I$PREFIX/include CFLAGS="-O3 -arch arm64 -arch armv7 -isysroot `xcrun -sdk iphoneos --show-sdk-path` -fembed-bitcode -mios-version-min=8.0" LDFLAGS="-L$PREFIX/lib -arch arm64 -arch armv7"
make -j4
make install
cd ..

tar xzf ../../libsrc/libogg-1.3.3.tar.gz
cd libogg-1.3.3
./configure --prefix=$PREFIX --host=arm-apple-darwin --disable-shared CFLAGS="-O3 -arch arm64 -arch armv7 -isysroot `xcrun -sdk iphoneos --show-sdk-path` -fembed-bitcode -mios-version-min=8.0" LDFLAGS="-arch arm64 -arch armv7"
make -j4
make install
cd ..

tar xzf ../../libsrc/libvorbis-1.3.6.tar.gz
cd libvorbis-1.3.6
sed 's/-force_cpusubtype_ALL//' configure > configure.new
mv configure.new configure
chmod +x configure
./configure --prefix=$PREFIX --host=arm-apple-darwin --disable-shared --with-ogg-includes=$PREFIX/include --with-ogg-libraries=$PREFIX/lib CFLAGS="-O3 -arch arm64 -arch armv7 -isysroot `xcrun -sdk iphoneos --show-sdk-path` -fembed-bitcode -mios-version-min=8.0" LDFLAGS="-arch arm64 -arch armv7"
make -j4
make install
cd ..

tar xzf ../../libsrc/freetype-2.9.1.tar.gz
cd freetype-2.9.1
sed -e 's/FONT_MODULES += type1//' \
    -e 's/FONT_MODULES += cff//' \
    -e 's/FONT_MODULES += cid//' \
    -e 's/FONT_MODULES += pfr//' \
    -e 's/FONT_MODULES += type42//' \
    -e 's/FONT_MODULES += winfonts//' \
    -e 's/FONT_MODULES += pcf//' \
    -e 's/FONT_MODULES += bdf//' \
    -e 's/FONT_MODULES += pshinter//' \
    -e 's/FONT_MODULES += raster//' \
    -e 's/FONT_MODULES += psaux//' \
    -e 's/FONT_MODULES += psnames//' \
    < modules.cfg > modules.cfg.new
mv modules.cfg.new modules.cfg
./configure --prefix=$PREFIX --host=arm-apple-darwin --disable-shared --with-png=no --with-zlib=no --with-harfbuzz=no --with-bzip2=no CFLAGS="-O3 -arch arm64 -arch armv7 -isysroot `xcrun -sdk iphoneos --show-sdk-path` -fembed-bitcode -mios-version-min=8.0" LDFLAGS="-arch arm64 -arch armv7"
make -j4
make install
cd ..

cd ..
rm -rf tmp
