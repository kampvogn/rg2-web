#!/bin/sh
# Build the headless RG2 exporter (tools/rg2export) as a 32-bit x86 Linux
# program: clang compiles the editor's MS-style inline assembly unchanged.
#   tools/build-exporter.sh
#   tools/rg2export rg2/flybye/raum102c.tg2     # writes export.dat here
set -e
cd "$(dirname "$0")/.."
docker build -q -t rg2-i386 tools/docker >/dev/null
SRC="animbase cmpbase engine exporter exportTool frOpGraph geobase plugbase texbase undoBuffer viruzII bezier curve
     expression math3d_2 memmgr stream sync tool tstream compose geocombine geofilter geogenerate geosys texcombine
     texfilter texgenerate texsys WindowMapper ViruzII/v2mconv ViruzII/sounddef
     export_main debug_headless directx_headless loadertune_headless"
tools/try.sh $SRC | grep -v " 0$" && { echo "compile errors, see tools/err/"; exit 1; } || true
OBJ=$(for f in $SRC; do printf "tools/err/%s.o " "$(basename $f)"; done)
tools/link.sh $OBJ >/dev/null || true
[ -x tools/rg2export ] && echo "built tools/rg2export" || { cat tools/err/link.err; exit 1; }
