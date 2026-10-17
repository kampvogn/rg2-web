// kampvogn.dk: headless stand-in for the editor's Direct3D layer (Directx.cpp).
// The exporter never draws; buffers are plain memory so code that fills them works.
#include "stdafx.h"

namespace fvs
{
  IDirect3D9 *d3d = 0;
  IDirect3DDevice9 *d3ddev = 0;
  viewportStruct viewport;
  config conf;

  struct vbSlice::iVertexBuffer { char *mem; sInt size; };
  struct ibSlice::iIndexBuffer { char *mem; sInt size; };

  baseTexture::baseTexture() : created(sFALSE), resId(0) {}
  baseTexture::~baseTexture() {}
  texture::texture() {}
  texture::texture(sU32 w_, sU32 h_, sU32 flags) { w = rw = w_; h = rh = h_; }
  texture::~texture() {}
  void *texture::getResID() const { return 0; }
  texture::operator IDirect3DTexture9 *() const { return 0; }
  void texture::upload(const sU8 *) {}

  static void *grow(char *&mem, sInt &size, sInt want) { if (want > size) { mem = (char *)realloc(mem, want); size = want; } return mem; }

  vbSlice::vbSlice(sInt nVerts, sBool dyn, sU32 VF, sInt streamInd) { ivb = new iVertexBuffer(); nVertices = nVerts; FVF = VF; grow(ivb->mem, ivb->size, nVerts * 64); }
  vbSlice::~vbSlice() { if (ivb) { free(ivb->mem); delete ivb; } }
  void *vbSlice::lock(sInt start, sInt size) { return ivb->mem; }
  void vbSlice::unlock() {}
  void vbSlice::resize(sInt n) { nVertices = n; grow(ivb->mem, ivb->size, n * 64); }
  sU32 vbSlice::use() const { return 0; }

  ibSlice::ibSlice(sInt nInds, sBool dyn) { iib = new iIndexBuffer(); nIndices = nInds; grow(iib->mem, iib->size, nInds * 2); }
  ibSlice::~ibSlice() { if (iib) { free(iib->mem); delete iib; } }
  void *ibSlice::lock(sInt start, sInt size) { return iib->mem; }
  void ibSlice::unlock() {}
  void ibSlice::resize(sInt n) { nIndices = n; grow(iib->mem, iib->size, n * 2); }
  sU32 ibSlice::use(sU32) const { return 0; }

  void setTransform(sU32, const fr::matrix &) {}
  void drawIndexedPrimitive(sU32, sU32, sU32, sU32, sU32) {}
}

sInt g_lastCompositingOp = 0; // TimelineWindow.cpp in the editor

namespace fvs
{
  void clear(sU32, sU32, sF32, sU32) {}
  void drawPrimitive(sU32, sU32, sU32) {}
  void setViewport(sU32, sU32, sU32, sU32) {}
}

// ViruzII playback (soundsys) is not needed to export
#include "soundsys.h"
void __stdcall ssInit(sU8 *, void *) {}
void __stdcall ssClose() {}
void __stdcall ssPlay() {}
void __stdcall ssPlayFrom(sS32) {}
void __stdcall ssStop() {}
sS32 __stdcall ssGetTime() { return 0; }
sS32 __stdcall ssGetSongTime() { return 0; }
sU32 __stdcall ssCalcPositions(sS32 **dest, sU8 *) { if (dest) *dest = 0; return 0; }
