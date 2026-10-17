// Differential test: the MMX/x86 asm blocks of rg2/Viewer/texop.cpp (copied
// verbatim) against their C versions in texop_c.h, on random input.
// Built and run as 32-bit x86 by tools/test-texop.sh.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned long long sU64;
#include "texop_c.h"

static sU32 rng = 12345;
static sU32 rnd() { rng = rng * 1664525u + 1013904223u; return rng >> 8; }
static int fails = 0;
static void check(const char *what, const void *a, const void *b, size_t n)
{
  if (memcmp(a, b, n)) { printf("MISMATCH %s\n", what); fails++; }
}

// ---- asm versions

static sS32 hue_asm(sS32 d, sS32 max, sS32 min, sS32 h, sS32 hv)
{
  __asm
  {
    mov     eax, [d];
    mov     edx, eax;
    shl     eax, 16;
    sar     edx, 16;
    mov     ebx, [max];
    sub     ebx, [min];
    inc     ebx;
    idiv    ebx;
    mov     ebx, [h];
    shl     ebx, 17;
    add     eax, [hv];
    add     eax, ebx;
    mov     [h], eax;
  }
  return h;
}

static void add_asm(sU16 *dst, const sU16 *src, sInt cnt, sInt gain)
{
  __asm
  {
    mov       esi, [src];
    mov       edi, [dst];
    mov       ecx, [cnt];
    lea       esi, [esi + ecx * 8];
    lea       edi, [edi + ecx * 8];
    neg       ecx;
    movd      mm2, [gain];
    punpcklwd mm2, mm2;
    punpcklwd mm2, mm2;
lp1:
    movq      mm0, [edi + ecx * 8];
    movq      mm1, [esi + ecx * 8];
    pmulhw    mm1, mm2;
    psllw     mm1, 1;
    paddsw    mm0, mm1;
    movq      [edi + ecx * 8], mm0;
    inc       ecx;
    jnz       lp1;
    emms;
  }
}

// the tool's Add (texcombine.cpp) for negative gains: every pixel, from
// cnt - 1 down to 0 (the viewer's copy of this loop stops after one pixel)
static void sub_asm(sU16 *dst, const sU16 *src, sInt cnt, sInt gain)
{
  cnt--;
  __asm
  {
    mov       esi, [src];
    mov       edi, [dst];
    mov       ecx, [cnt];
    movd      mm2, [gain];
    punpcklwd mm2, mm2;
    punpcklwd mm2, mm2;
lp2:
    movq      mm0, [edi + ecx * 8];
    movq      mm1, [esi + ecx * 8];
    pmulhw    mm1, mm2;
    psllw     mm1, 1;
    psubusw   mm0, mm1;
    movq      [edi + ecx * 8], mm0;
    dec       ecx;
    jns       lp2;
    emms;
  }
}

static void mul_asm(sU16 *dst, const sU16 *src, sInt cnt)
{
  __asm
  {
    mov     esi, [src];
    mov     edi, [dst];
    mov     ecx, [cnt];
    lea     esi, [esi + ecx * 8];
    lea     edi, [edi + ecx * 8];
    neg     ecx;
lp:
    movq    mm0, [esi + ecx * 8];
    pmulhw  mm0, [edi + ecx * 8];
    psllw   mm0, 1;
    movq    [edi + ecx * 8], mm0;
    inc     ecx;
    jnz     lp;
    emms;
  }
}

static sS32 imul16_asm(sS32 a, sS32 b)
{
  __asm
  {
    mov   eax, [a];
    imul  [b];
    shrd  eax, edx, 16;
    mov   [a], eax;
  }
  return a;
}

static sU32 srandom2_asm(sU32 n)
{
  __asm
  {
    mov   ecx, [n];
    mov   ebx, ecx;
    shl   ebx, 13;
    xor   ecx, ebx;
    mov   ebx, ecx;
    mov   eax, 15731;
    imul  ebx, ebx;
    imul  ebx, eax;
    add   ebx, 789221;
    imul  ecx, ebx;
    add   ecx, 1376312589;
    mov   eax, 2048;
    and   ecx, 07fffffffh;
    shr   ecx, 19;
    sub   eax, ecx;
    mov   [n], eax;
  }
  return n;
}

static void crystal_asm(sU16 *dptr, sInt width, sInt height, sU32 sed, sU32 cnt, sU32 col1, sU32 col2)
{
  sU8   randBuf[512];
  static const sU32 _MMMax = 0x7fffffff;
  const sU32 _MM256 = (height << 16) | width, temp = (_MM256 >> 1) & 0x7fff7fff;
  const sU64 _MM128 = ((sU64)temp << 32) | temp;
  const sU64 _MMMask = ((height - 1) << 16) | (width - 1);

  __asm
  {
    mov       edx, [sed];
    mov       esi, [dptr];
    mov       ecx, 511;
randomloop:
    imul      edx, 015a4e35h;
    inc       edx;
    mov       eax, edx;
    shr       eax, 16;
    mov       randBuf[ecx], al;
    dec       ecx;
    jns       randomloop;

    emms;
    pxor      mm7, mm7;
    xor       ecx, ecx;
    xor       edx, edx;
yloop:
    and       ecx, 0ffff0000h;
xloop:
    mov       edi, [cnt];
    dec       edi
    movd      mm2, ecx;
    movd      mm0, [_MMMax];
    movd      mm6, [_MM256];
    movq      mm5, mm2;
    xor       ebx, ebx;
    dec       ebx;
    push      esi;
tryloop:
    movd      mm1, randBuf[edi*2];
    movq      mm2, mm5;
    pand      mm1, [_MMMask];
    psubw     mm2, mm1;
    movq      mm1, mm0;
    movq      mm3, mm2;
    psraw     mm3, 15;
    pxor      mm2, mm3;
    psubsw    mm2, mm3;
    movq      mm3, mm2;
    pcmpgtw   mm3, [_MM128];
    movq      mm4, mm6;
    psubw     mm4, mm2;
    pand      mm4, mm3;
    pandn     mm3, mm2;
    por       mm3, mm4;
    pmaddwd   mm3, mm3;
    pcmpgtd   mm0, mm3;
    movd      eax, mm3;
    pand      mm3, mm0;
    pandn     mm0, mm1;
    por       mm0, mm3;
    cmp       eax, ebx;
    cmovb     ebx, eax;
    cmovb     esi, edi;
    dec       edi;
    jns       tryloop;

    mov       ebx, esi;
    pop       esi;
    movd      eax, mm0;
    cmp       eax, edx;
    cmova     edx, eax;
    mov       [esi], eax;
    add       esi, 8;
    inc       ecx;
    movzx     eax, cx;
    cmp       eax, [width];
    jne       xloop;
    add       ecx, 010000h;
    mov       eax, ecx;
    shr       eax, 16;
    cmp       eax, [height];
    jne       yloop;

    mov       ecx, [width];
    imul      ecx, [height];
    mov       [width], ecx;
    mov       ecx, 0ffffh;
    mov       edi, edx;
    movd      mm0, [col1];
    movd      mm1, [col2];
    punpcklbw mm0, mm7;
    punpcklbw mm1, mm7;
    psllw     mm0, 7;
    psllw     mm1, 7;
    psubw     mm1, mm0;
contrastloop:
    sub       esi, 8;
    mov       eax, [esi];
    mov       edx, eax;
    shl       eax, 16;
    sar       edx, 16;
    idiv      edi;
    xor       eax, -1;
    add       eax, ecx;
    imul      eax;
    shrd      eax, edx, 16;
    xor       eax, -1;
    add       eax, ecx;
    shr       eax, 1;
    movd      mm2, eax;
    punpcklwd mm2, mm2;
    punpcklwd mm2, mm2;
    pmulhw    mm2, mm1;
    psllw     mm2, 1;
    paddw     mm2, mm0;
    movq      [esi], mm2;
    mov       [esi+6], ax;
    dec       [width];
    jnz       contrastloop;
    emms;
  };
}

static void blur_asm(sU16 *dst, sU32 w, sU32 h, sUInt radius, sInt wrap)
{
  static sU16 pbuf[(1024+256)*4];
  if (2*radius>=w) radius=(w-1)/2;
  sU32 sc=65536/(2*radius+1);
  memset(pbuf, 0, sizeof(pbuf));
  while (h--)
  {
    memcpy(pbuf+radius*4, dst, w*4*sizeof(sU16));
    const sU16 *src=pbuf+radius*4;
    static const sU64 msk1=0x0000ffff0000ffff;
    __asm
    {
      mov       esi, offset pbuf;
      pxor      mm0, mm0;
      pxor      mm1, mm1;
      pxor      mm3, mm3;
      mov       ecx, [radius];
      shl       ecx, 1;
sumlp:
      movq      mm4, [esi];
      add       esi, 8;
      movq      mm5, mm4;
      punpcklwd mm4, mm3;
      dec       ecx;
      punpckhwd mm5, mm3;
      paddd     mm0, mm4;
      paddd     mm1, mm5;
      jns       sumlp;

      mov       esi, [src];
      mov       edi, [dst];
      mov       ecx, [w];
      mov       ebx, [radius];
      shl       ebx, 3;
      lea       esi, [esi+ecx*8];
      lea       edi, [edi+ecx*8];
      lea       edx, [esi+ebx+8];
      sub       esi, ebx;
      neg       ecx;
      movd      mm2, [sc];
      punpcklwd mm2, mm2;
      punpcklwd mm2, mm2;
pixlp:
      movq      mm4, mm0;
      movq      mm5, mm1;
      movq      mm6, mm0;
      movq      mm7, mm1;
      psrld     mm4, 16;
      psrld     mm5, 16;
      psrlw     mm6, 1;
      psrlw     mm7, 1;
      pmulhw    mm6, mm2;
      pmulhw    mm7, mm2;
      pmullw    mm4, mm2;
      pmullw    mm5, mm2;
      pand      mm6, [msk1];
      pand      mm7, [msk1];
      psllw     mm6, 1;
      psllw     mm7, 1;
      paddd     mm4, mm6;
      paddd     mm5, mm7;
      packssdw  mm4, mm5;
      movq      [edi+ecx*8], mm4;
      movq      mm4, [edx+ecx*8];
      movq      mm6, [esi+ecx*8];
      movq      mm5, mm4;
      movq      mm7, mm6;
      punpcklwd mm4, mm3;
      punpckhwd mm5, mm3;
      punpcklwd mm6, mm3;
      punpckhwd mm7, mm3;
      paddd     mm0, mm4;
      paddd     mm1, mm5;
      psubd     mm0, mm6;
      psubd     mm1, mm7;
      inc       ecx;
      jnz       pixlp;
      mov       [dst], edi;
      emms;
    }
  }
}

static void blur_c(sU16 *dst, sU32 w, sU32 h, sUInt radius)
{
  static sU16 pbuf[(1024+256)*4];
  if (2*radius>=w) radius=(w-1)/2;
  sU32 sc=65536/(2*radius+1);
  memset(pbuf, 0, sizeof(pbuf));
  while (h--)
  {
    memcpy(pbuf+radius*4, dst, w*4*sizeof(sU16));
    texop_c::blurRow(pbuf, pbuf+radius*4, dst, w, radius, sc);
    dst += w*4;
  }
}

int main()
{
  for (int t = 0; t < 2000; t++)
  {
    sS32 min = rnd() & 0x7fff, max = min + (rnd() & 0x7fff), d = (sS32)(rnd() & 0xffff) - 32768, h = rnd() & 3, hv = rnd() & 0x3ffff;
    sS32 a = hue_asm(d, max, min, h, hv), b = texop_c::hueShift(d, max, min, h, hv);
    check("hue", &a, &b, 4);
    sS32 x = (sS32)rnd() * 37, y = (sS32)rnd() * 11;
    a = imul16_asm(x, y); b = texop_c::imul16(x, y); check("imul16", &a, &b, 4);
    sU32 n = rnd() * 977u, ra = srandom2_asm(n), rb = texop_c::srandom2(n); check("srandom2", &ra, &rb, 4);
  }
  const int cnt = 300;
  static sU16 src[cnt * 4], d1[cnt * 4], d2[cnt * 4];
  for (int t = 0; t < 200; t++)
  {
    for (int i = 0; i < cnt * 4; i++) { src[i] = rnd(); d1[i] = d2[i] = rnd(); }
    sInt gain = (sInt)(rnd() % 127) * 32767 / 127;
    add_asm(d1, src, cnt, gain); texop_c::addGain(d2, src, cnt, gain); check("add", d1, d2, sizeof d1);
    sub_asm(d1, src, cnt, gain); texop_c::subGain(d2, src, cnt, gain); check("sub", d1, d2, sizeof d1);
    mul_asm(d1, src, cnt); texop_c::multiply(d2, src, cnt); check("mul", d1, d2, sizeof d1);
  }
  static sU16 c1[256 * 256 * 4], c2[256 * 256 * 4];
  int sizes[][2] = {{64, 64}, {128, 32}, {256, 256}};
  for (int s = 0; s < 3; s++)
    for (int t = 0; t < 4; t++)
    {
      int w = sizes[s][0], h = sizes[s][1];
      sU32 seed = rnd() & 0xffff, count = 1 + rnd() % 255, col1 = rnd() & 0xffffff, col2 = rnd() & 0xffffff;
      memset(c1, 0, sizeof c1); memset(c2, 0, sizeof c2);
      crystal_asm(c1, w, h, seed, count, col1, col2);
      texop_c::crystal(c2, w, h, seed, count, col1, col2);
      check("crystal", c1, c2, w * h * 8);
      for (int i = 0; i < w * h * 4; i++) c1[i] = c2[i] = rnd() & 0x7fff;
      sU32 r = 1 + rnd() % 20;
      blur_asm(c1, w, h, r, 0); blur_c(c2, w, h, r);
      check("blur", c1, c2, w * h * 8);
    }
  printf(fails ? "%d FAILED\n" : "all texop routines match the asm\n", fails);
  return fails != 0;
}
