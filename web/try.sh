#!/bin/sh
# Compile the RG2 viewer to WebAssembly object files; errors into web/err/<file>.err
cd "$(dirname "$0")/.." && mkdir -p web/err
docker run --rm -u $(id -u):$(id -g) -e HOME=/tmp -e EM_CACHE=/tmp/emcache -v $PWD:/w -w /w/rg2/Viewer emscripten/emsdk:4.0.10 sh -c '
for f in "$@"; do
  em++ -std=gnu++14 -fms-extensions -fdeclspec -w -Wno-everything -D_WEBPLAYER -include /w/web/compat.h \
    -I/w/web/shim -I/w/d3d9-webgl -I/w/rg2/Viewer -I/w/rg2 -ferror-limit=0 -c $f.cpp -o /w/web/err/$f.o 2> /w/web/err/$f.err
  echo "$f $(grep -c "error:" /w/web/err/$f.err)"
done' sh "$@"
