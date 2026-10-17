# RG2 intros in the browser

Farbrausch's RG2 ("RauschGenerator 2") was the tool behind three of their
productions from 2001–2002, all released with source and data in
[farbrausch/fr_public](https://github.com/farbrausch/fr_public/tree/master/RG2):

- **fr-013: flybye** – invitation demo for The Party 2001
- **fr-022: ein.schlag** – 64k intro, 2nd at Mekka & Symposium 2002
- **fr-029: dopplerdefekt** – 64k intro, 2002

This repository works towards running them in the browser, the same way as
[candytron](https://kampvogn.dk/demos/candytron/)
([source](https://github.com/kampvogn/candytron-web)): built from the original
code to WebAssembly, Direct3D through
[d3d9-webgl](https://github.com/kampvogn/d3d9-webgl) (submodule).

## Status

1. **Export – done.** The player (`rg2/Viewer`) does not read the project files;
   it plays one block of data that the editor's *Make Demo* writes. The editor
   is a Windows (WTL, Direct3D) application, so `tools/rg2export` builds its
   loading and exporting code without the GUI, as a 32-bit x86 Linux program
   (clang keeps the operators' MS-style inline assembly), and writes
   `export.dat` for each of the three projects.
2. **Player – done.** `rg2/Viewer` runs as WebAssembly (`web/build.sh`):
   the MMX/x86 assembly is ported to C (`rg2/Viewer/texop_c.h`, checked
   bit for bit against the original by `tools/test-texop.sh`), the V2 synth
   and DirectSound by V2MPlayer on SDL audio (`web/src`), Direct3D 8 by a thin
   shim over d3d9-webgl (`web/shim/d3d8.h`), GDI text by a 2D canvas.
3. **Compared with recordings of the released intros**, frame by frame.
   Where the published viewer source differs from what the intros look like,
   the port follows the intros (all marked `kampvogn.dk` in the source):
   - materials set to *sort* (transparent text panels, ein.schlag's credits)
     are painted after the solid objects, back to front, as in the tool;
   - Add with a negative gain subtracts every pixel, as in the tool (the
     viewer's loop stopped after one), and `pow(0, y)` is 0, as in the
     intros' own runtime (`Rtlib.asm`) – together they give ein.schlag's
     Buddha his head;
   - a spot light with falloff 0 lights all around (ein.schlag's blue cave).

   Known differences: flybye's invitation text (the scrolling party info) is
   not part of the published project, and ein.schlag's candle flames come out
   as white squares. The ein.schlag and dopplerdefekt data are not the exact
   versions in the recordings (other credits, some camera moves).

The page takes `?t=<seconds>` to start somewhere in the middle (the timeline
is run up to there, the music plays from there). A build with
`CFLAGS_EXTRA=-DRG2_TRACE` logs operators, materials, lights and texture
uploads to the console.

## Exporting

Needs Docker.

```sh
git clone --recursive https://github.com/kampvogn/rg2-web && cd rg2-web
tools/build-exporter.sh                       # tools/rg2export
mkdir -p out/flybye && cd out/flybye
docker run --rm -u $(id -u):$(id -g) -v $PWD/../..:/w -v $PWD:/o -w /o rg2-i386 \
  /w/tools/rg2export /w/rg2/flybye/raum102c.tg2   # → export.dat
```

The other projects are `rg2/einschlag/einschlagrc21c.tg2` and
`rg2/dopplerdefekt/stereo10b_b.rg21`.

What changed in `rg2/` (marked `kampvogn.dk`): `stdafx.h` includes
`shim/stdafx_headless.h` when built with `_HEADLESS`; the exporter skips
linking the viewer executable; `gaussKernel` is not static (inline assembly
refers to it by name); a token paste in `stream.h`. New files:
`export_main.cpp`, `debug_headless.cpp`, `directx_headless.cpp` (no-op
Direct3D layer and sound), `loadertune_headless.cpp`. `shim/` has the Win32
stand-ins.

## Licenses

RG2's code is in the public domain (`rg2/LICENSE`). The projects and music are
by Farbrausch: flybye under CC-BY 3.0, ein.schlag and dopplerdefekt under
CC-BY-SA 3.0 (see the `LICENSE` files in their directories). The additions
here are under the same terms as the code they belong to.
