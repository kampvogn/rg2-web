#!/bin/sh
# Compile RG2 sources headless as 32-bit x86 (clang, MS inline asm); errors into tools/err/<file>.err
cd "$(dirname "$0")/.." && mkdir -p tools/err
docker run --rm -u $(id -u):$(id -g) -e HOME=/tmp -v $PWD:/w -w /w/rg2 rg2-i386 sh -c '
for f in "$@"; do src=$f.cpp; [ -f $src ] || src=ViruzII/$f.cpp; f=$(basename $f);
  clang++ -std=gnu++14 -fms-extensions -fms-compatibility -fms-compatibility-version=19 -fasm-blocks -fno-strict-aliasing -w -Wno-everything -D_HEADLESS \
    -I/w/shim -I/w/d3d9-webgl -I/w/rg2 -I/w/rg2/ViruzII -ferror-limit=0 -c $src -o /w/tools/err/$f.o 2> /w/tools/err/$f.err
  echo "$f $(grep -c "error:" /w/tools/err/$f.err)"
done' sh "$@"
