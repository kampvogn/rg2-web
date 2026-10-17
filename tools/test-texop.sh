#!/bin/sh
# Compare texop.cpp's asm with its C versions (texop_c.h) on 32-bit x86
cd "$(dirname "$0")/.." && docker build -q -t rg2-i386 tools/docker >/dev/null
docker run --rm -u $(id -u):$(id -g) -v $PWD:/w -w /w rg2-i386 sh -c \
  'clang++ -O0 -fno-omit-frame-pointer -fno-pic -no-pie -fms-extensions -fasm-blocks -w -I/w/rg2 -I/w/rg2/Viewer tools/test-texop.cpp -o /tmp/t && /tmp/t'
