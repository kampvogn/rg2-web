// kampvogn.dk: Direct3D 8 as far as the RG2 viewer uses it, on top of
// d3d9-webgl. Textures, surfaces and buffers are the D3D9 interfaces under
// their D3D8 names; IDirect3D8 and IDirect3DDevice8 wrap the D3D9 objects for
// the few calls whose signatures changed between 8 and 9.
#pragma once
#include "d3d9.h"

// ---- constants D3D8 code uses that d3d9.h does not declare
#define D3DERR_OUTOFVIDEOMEMORY ((HRESULT)0x8876017C)
#define D3DSGR_NO_CALIBRATION 0
#define D3DFMT_X1R5G5B5 ((D3DFORMAT)24)
#define D3DFMT_X4R4G4B4 ((D3DFORMAT)30)
#define D3DFMT_R3G3B2 ((D3DFORMAT)27)
#define D3DFMT_A8R3G3B2 ((D3DFORMAT)29)
#define D3DFMT_A8 ((D3DFORMAT)28)
#define D3DFMT_L8 ((D3DFORMAT)50)
#define D3DFMT_A8L8 ((D3DFORMAT)51)
#define D3DFMT_A4L4 ((D3DFORMAT)52)
#define D3DFMT_D16_LOCKABLE ((D3DFORMAT)70)
#define D3DFMT_D15S1 ((D3DFORMAT)73)
#define D3DFMT_D24X8 ((D3DFORMAT)77)
#define D3DFMT_D24X4S4 ((D3DFORMAT)79)
#define D3DFVF_POSITION_MASK 0x00E
#define D3DFVF_XYZB1 0x006
#define D3DFVF_XYZB3 0x00A
#define D3DFVF_XYZB4 0x00C
#define D3DFVF_XYZB5 0x00E
#define D3DFVF_PSIZE 0x020
#define D3DFVF_TEXCOUNT_MASK 0xF00
#define D3DFVF_TEXCOUNT_SHIFT 8
#define D3DFVF_TEXTUREFORMAT1 3
#define D3DFVF_TEXTUREFORMAT2 0
#define D3DFVF_TEXTUREFORMAT3 1
#define D3DFVF_TEXTUREFORMAT4 2
#define D3DPTEXTURECAPS_POW2 0x00000002L
#define D3DPTEXTURECAPS_SQUAREONLY 0x00000020L
#define D3DPTEXTURECAPS_MIPMAP 0x00004000L
#define D3DPTEXTURECAPS_CUBEMAP 0x00000800L
#define D3DPTEXTURECAPS_MIPCUBEMAP 0x00010000L
#define D3DPTEXTURECAPS_CUBEMAP_POW2 0x00020000L
typedef DWORD D3DCUBEMAP_FACES;
typedef D3DVIEWPORT9 D3DVIEWPORT8;
#ifndef LPRECT
typedef RECT *LPRECT;
#endif
static inline void *SetCursor(void *) { return 0; }

// In D3D8 the sampler states are texture stage states
#define D3DTSS_ADDRESSU ((D3DTEXTURESTAGESTATETYPE)13)
#define D3DTSS_ADDRESSV ((D3DTEXTURESTAGESTATETYPE)14)
#define D3DTSS_BORDERCOLOR ((D3DTEXTURESTAGESTATETYPE)15)
#define D3DTSS_MAGFILTER ((D3DTEXTURESTAGESTATETYPE)16)
#define D3DTSS_MINFILTER ((D3DTEXTURESTAGESTATETYPE)17)
#define D3DTSS_MIPFILTER ((D3DTEXTURESTAGESTATETYPE)18)
#define D3DTSS_MIPMAPLODBIAS ((D3DTEXTURESTAGESTATETYPE)19)
#define D3DTSS_MAXMIPLEVEL ((D3DTEXTURESTAGESTATETYPE)20)
#define D3DTSS_MAXANISOTROPY ((D3DTEXTURESTAGESTATETYPE)21)
#define D3DTSS_ADDRESSW ((D3DTEXTURESTAGESTATETYPE)25)

typedef IDirect3DSurface9 IDirect3DSurface8;
typedef IDirect3DBaseTexture9 IDirect3DBaseTexture8;
typedef IDirect3DTexture9 IDirect3DTexture8;
typedef IDirect3DVertexBuffer9 IDirect3DVertexBuffer8;
typedef IDirect3DIndexBuffer9 IDirect3DIndexBuffer8;
class IDirect3DCubeTexture8;

typedef D3DLIGHT9 D3DLIGHT8;
typedef D3DMATERIAL9 D3DMATERIAL8;

struct D3DCAPS8 { DWORD DevCaps; DWORD TextureCaps; };

class IDirect3DDevice8
{
  IDirect3DDevice9 *d;
  INT base = 0;              // SetIndices' BaseVertexIndex, for DrawIndexedPrimitive
public:
  explicit IDirect3DDevice8(IDirect3DDevice9 *dev) : d(dev) {}
  ULONG Release() { d->Release(); delete this; return 0; }

  HRESULT SetVertexShader(DWORD fvf) { return d->SetFVF(fvf); }
  HRESULT SetStreamSource(UINT n, IDirect3DVertexBuffer8 *vb, UINT stride) { return d->SetStreamSource(n, vb, 0, stride); }
  HRESULT SetIndices(IDirect3DIndexBuffer8 *ib, UINT baseVertex) { base = (INT)baseVertex; return d->SetIndices(ib); }
  HRESULT DrawPrimitive(D3DPRIMITIVETYPE t, UINT start, UINT count) { return d->DrawPrimitive(t, start, count); }
  HRESULT DrawIndexedPrimitive(D3DPRIMITIVETYPE t, UINT minIndex, UINT numVerts, UINT startIndex, UINT count)
  { return d->DrawIndexedPrimitive(t, base, minIndex, numVerts, startIndex, count); }

  HRESULT CreateTexture(UINT w, UINT h, UINT levels, DWORD usage, D3DFORMAT fmt, D3DPOOL pool, IDirect3DTexture8 **tex)
  { return d->CreateTexture(w, h, levels, usage, fmt, pool, tex, 0); }
  HRESULT CreateCubeTexture(UINT, UINT, DWORD, D3DFORMAT, D3DPOOL, IDirect3DCubeTexture8 **) { return D3DERR_INVALIDCALL; }
  HRESULT CreateVertexBuffer(UINT len, DWORD usage, DWORD fvf, D3DPOOL pool, IDirect3DVertexBuffer8 **vb)
  { return d->CreateVertexBuffer(len, usage, fvf, pool, vb, 0); }
  HRESULT CreateIndexBuffer(UINT len, DWORD usage, D3DFORMAT fmt, D3DPOOL pool, IDirect3DIndexBuffer8 **ib)
  { return d->CreateIndexBuffer(len, usage, fmt, pool, ib, 0); }
  HRESULT CreateDepthStencilSurface(UINT w, UINT h, D3DFORMAT fmt, D3DMULTISAMPLE_TYPE ms, IDirect3DSurface8 **s)
  { return d->CreateDepthStencilSurface(w, h, D3DFMT_D24S8, ms, 0, FALSE, s, 0); }
  HRESULT UpdateTexture(IDirect3DBaseTexture8 *, IDirect3DBaseTexture8 *) { return D3D_OK; } // animated textures only

  HRESULT GetBackBuffer(UINT i, D3DBACKBUFFER_TYPE type, IDirect3DSurface8 **s) { return d->GetBackBuffer(0, i, type, s); }
  HRESULT SetRenderTarget(IDirect3DSurface8 *rt, IDirect3DSurface8 *z)
  {
    HRESULT hr = d->SetRenderTarget(0, rt);
    d->SetDepthStencilSurface(z);
    return hr;
  }
  HRESULT SetGammaRamp(DWORD, const D3DGAMMARAMP *) { return D3D_OK; }

  HRESULT SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, DWORD value)
  {
    switch ((int)type) {
      case 13: return d->SetSamplerState(stage, D3DSAMP_ADDRESSU, value);
      case 14: return d->SetSamplerState(stage, D3DSAMP_ADDRESSV, value);
      case 25: return d->SetSamplerState(stage, D3DSAMP_ADDRESSW, value);
      case 15: return d->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)4, value);
      case 16: return d->SetSamplerState(stage, D3DSAMP_MAGFILTER, value);
      case 17: return d->SetSamplerState(stage, D3DSAMP_MINFILTER, value);
      case 18: return d->SetSamplerState(stage, D3DSAMP_MIPFILTER, value);
      case 19: return d->SetSamplerState(stage, D3DSAMP_MIPMAPLODBIAS, value);
      case 20: return d->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)9, value);
      case 21: return d->SetSamplerState(stage, (D3DSAMPLERSTATETYPE)10, value);
    }
    return d->SetTextureStageState(stage, type, value);
  }
  HRESULT SetRenderState(D3DRENDERSTATETYPE s, DWORD v) { return d->SetRenderState(s, v); }
  HRESULT SetTransform(D3DTRANSFORMSTATETYPE s, const D3DMATRIX *m) { return d->SetTransform(s, m); }
  HRESULT SetTexture(DWORD stage, IDirect3DBaseTexture8 *t) { return d->SetTexture(stage, (IDirect3DTexture9 *)t); }
  HRESULT SetLight(DWORD i, const D3DLIGHT9 *l) { return d->SetLight(i, l); }
  HRESULT LightEnable(DWORD i, BOOL on) { return d->LightEnable(i, on); }
  HRESULT SetMaterial(const D3DMATERIAL9 *m) { return d->SetMaterial(m); }
  HRESULT SetViewport(const D3DVIEWPORT9 *v) { return d->SetViewport(v); }
  HRESULT Clear(DWORD n, const D3DRECT *r, DWORD flags, D3DCOLOR c, float z, DWORD s) { return d->Clear(n, r, flags, c, z, s); }
  HRESULT BeginScene() { return d->BeginScene(); }
  HRESULT EndScene() { return d->EndScene(); }
  HRESULT Present(const RECT *a, const RECT *b, HWND w, const void *r) { return d->Present(a, b, w, r); }
};

class IDirect3D8
{
  IDirect3D9 *d;
public:
  explicit IDirect3D8(IDirect3D9 *d3d) : d(d3d) {}
  ULONG Release() { d->Release(); delete this; return 0; }
  HRESULT CheckDeviceFormat(UINT a, D3DDEVTYPE t, D3DFORMAT af, DWORD u, D3DRESOURCETYPE r, D3DFORMAT f)
  {
    // WebGL has the formats the viewer probes for in its own way; say yes to
    // 32-bit colour and the D24S8 depth buffer, no to the rest
    if (u & D3DUSAGE_DEPTHSTENCIL) return f == D3DFMT_D24S8 ? D3D_OK : D3DERR_NOTAVAILABLE;
    return (f == D3DFMT_A8R8G8B8 || f == D3DFMT_X8R8G8B8) ? D3D_OK : D3DERR_NOTAVAILABLE;
  }
  HRESULT CheckDepthStencilMatch(UINT, D3DDEVTYPE, D3DFORMAT, D3DFORMAT, D3DFORMAT f) { return f == D3DFMT_D24S8 ? D3D_OK : D3DERR_NOTAVAILABLE; }
  HRESULT GetAdapterDisplayMode(UINT a, D3DDISPLAYMODE *m) { return d->GetAdapterDisplayMode(a, m); }
  HRESULT GetDeviceCaps(UINT, D3DDEVTYPE, D3DCAPS8 *c)
  {
    c->DevCaps = D3DDEVCAPS_HWTRANSFORMANDLIGHT;
    c->TextureCaps = D3DPTEXTURECAPS_MIPMAP; // WebGL 2: any size, mipmaps
    return D3D_OK;
  }
  HRESULT CreateDevice(UINT a, D3DDEVTYPE t, HWND w, DWORD flags, D3DPRESENT_PARAMETERS *pp, IDirect3DDevice8 **dev)
  {
    IDirect3DDevice9 *d9 = 0;
    HRESULT hr = d->CreateDevice(a, t, w, flags, pp, &d9);
    *dev = hr == D3D_OK ? new IDirect3DDevice8(d9) : 0;
    return hr;
  }
};

static inline IDirect3D8 *Direct3DCreate8(UINT sdk)
{
  IDirect3D9 *d = Direct3DCreate9(D3D_SDK_VERSION);
  return d ? new IDirect3D8(d) : 0;
}
