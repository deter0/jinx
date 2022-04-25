CC=clang;
CFLAGS="-Weverything -Wno-missing-prototypes -Wno-implicit-float-conversion -Wno-padded -Wno-double-promotion -Wno-extra-semi -Wno-cast-align -Wno-sign-compare -pedantic";
CLIBS=`pkg-config --libs --cflags x11 cairo`;

set -xe

$CC src/main.c $CFLAGS -o jinx $CLIBS
