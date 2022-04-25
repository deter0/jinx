CC=clang;
CFLAGS="-Weverything -Wno-padded -pedantic";
CLIBS=`pkg-config --libs --cflags x11 cairo`;

$CC src/main.c $CFLAGS -o jinx $CLIBS
