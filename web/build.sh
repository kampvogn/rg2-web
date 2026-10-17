#!/bin/sh
# Build the RG2 viewer (Farbrausch's player for flybye, ein.schlag and
# dopplerdefekt) to WebAssembly: web/build/rg2.js and rg2.wasm. The intros'
# data are tools/rg2export's export.dat files (web/build/<intro>.dat).
#   git submodule update --init && web/build.sh
set -e
cd "$(dirname "$0")/.."
OUT=${OUT:-web/build}
mkdir -p $OUT
docker run --rm -u $(id -u):$(id -g) -e HOME=/tmp -e EM_CACHE=/tmp/emcache -e OUT=$OUT -e EXTRA_LDFLAGS="${EXTRA_LDFLAGS:-}" -e OPT="${OPT:--O2}" -e CFLAGS_EXTRA="${CFLAGS_EXTRA:-}" \
  -v $PWD:/w -w /w ${IMAGE:-emscripten/emsdk:4.0.10} sh -c '
set -e
RG="-std=gnu++14 -fms-extensions -fdeclspec -fno-delete-null-pointer-checks -w $OPT ${CFLAGS_EXTRA:-} -include /w/web/compat.h -I/w/web/shim -I/w/d3d9-webgl -I/w/rg2/Viewer -I/w/rg2 -sUSE_SDL=2"
V2="-std=gnu++14 -fms-extensions -w $OPT -I/w/v2 -sUSE_SDL=2"
fail=0
for f in viewer_web Directx Engine cmpop Geoop geometry opdata opsys optable texop texture math3d_2 debug_web; do
  em++ $RG -c rg2/Viewer/$f.cpp -o $OUT/$f.o 2> $OUT/$f.err && echo "ok   $f" || { echo "FAIL $f"; fail=1; }
done
em++ $RG -c web/src/sound_web.cpp -o $OUT/sound_web.o 2> $OUT/sound_web.err && echo "ok   sound_web" || fail=1
for f in v2web v2synth; do em++ $V2 -c web/src/$f.cpp -o $OUT/$f.o 2> $OUT/$f.err && echo "ok   $f" || { echo "FAIL $f"; fail=1; }; done
em++ $V2 -c v2/v2mconv.cpp -o $OUT/v2mconv.o 2> $OUT/v2mconv.err && echo "ok   v2mconv" || fail=1
em++ -std=gnu++17 -O2 -w -I/w/d3d9-webgl -c d3d9-webgl/d3d9.cpp -o $OUT/d3d9.o 2> $OUT/d3d9.err && echo "ok   d3d9-webgl" || fail=1
[ $fail = 0 ]
em++ -O2 $OUT/*.o -o $OUT/rg2.js -sUSE_SDL=2 -sUSE_WEBGL2=1 -sFULL_ES3=1 -sMAX_WEBGL_VERSION=2 \
  -sASYNCIFY -sASYNCIFY_STACK_SIZE=1048576 -sSTACK_SIZE=8MB -sALLOW_MEMORY_GROWTH -sINITIAL_MEMORY=256MB \
  -sENVIRONMENT=web -sINVOKE_RUN=0 -sEXIT_RUNTIME=0 -sFORCE_FILESYSTEM -sEXPORTED_RUNTIME_METHODS=callMain,FS $EXTRA_LDFLAGS 2> $OUT/link.err \
  && echo "linked $(wc -c < $OUT/rg2.wasm) bytes" || { grep -o "undefined symbol: .*" $OUT/link.err | sort -u | head -40; exit 1; }'
