#!/bin/sh
# Link the headless exporter from tools/err/*.o (built by try.sh): tools/rg2export
cd "$(dirname "$0")/.." && docker run --rm -u $(id -u):$(id -g) -v $PWD:/w -w /w rg2-i386 sh -c \
  "clang++ -fuse-ld=lld -Wl,--allow-multiple-definition $* -o tools/rg2export 2> tools/err/link.err; grep -o 'undefined symbol: .*' tools/err/link.err | sort -u | head -80; grep -c 'undefined symbol' tools/err/link.err"
