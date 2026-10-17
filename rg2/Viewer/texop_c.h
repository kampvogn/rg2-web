// kampvogn.dk: the MMX and x86 assembly of texop.cpp in portable C, for
// WebAssembly. Each function does exactly what its asm block does, including
// wrap-around and saturation, so the generated textures come out the same
// (tools/test-texop.sh compares them with the original asm on x86).
// Pixels are four 16-bit channels (sU16[4], 8 bytes).
#ifndef __texop_c_h_
#define __texop_c_h_

#include "types.h"
#include <string.h>

namespace texop_c
{
  typedef unsigned long long u64;

  static inline sInt lane(u64 v, int i) { return (sInt)((v >> (i * 16)) & 0xffff); }
  static inline sInt s16(sInt x) { return (short)(x & 0xffff); }
  static inline sInt sat16(sInt x) { return x < -32768 ? -32768 : x > 32767 ? 32767 : x; }

  // hue: ((d << 16) / (max - min + 1)) + hv + (h << 17), with idiv truncation
  static inline sS32 hueShift(sS32 d, sS32 max, sS32 min, sS32 h, sS32 hv)
  {
    long long q = ((long long)d * 65536) / (long long)(max - min + 1);
    return (sS32)((sU32)(sS32)q + (sU32)hv + ((sU32)h << 17));
  }

  // dst += 2 * ((src * gain) >> 16), per channel, saturating (paddsw)
  static void addGain(sU16 *dst, const sU16 *src, sInt cnt, sInt gain)
  {
    const sInt g = s16(gain);
    for (sInt i = 0; i < cnt * 4; i++)
    {
      sInt m = ((s16(src[i]) * g) >> 16) << 1;
      dst[i] = (sU16)sat16(s16(dst[i]) + s16(m));
    }
  }

  // dst -= 2 * ((src * gain) >> 16), per channel, saturating at 0 (psubusw).
  // The viewer's asm for negative gains starts at -cnt but counts down
  // (dec/jns), so it stops after the first pixel; the tool's own Add
  // (texcombine.cpp) counts from cnt - 1 down to 0 and does every pixel, and
  // that is what the released intros show (ein.schlag's Buddha head: its
  // height map has the eyes and the background subtracted).
  static void subGain(sU16 *dst, const sU16 *src, sInt cnt, sInt gain)
  {
    const sInt g = s16(gain);
    for (sInt i = 0; i < cnt * 4; i++)
    {
      sInt m = (((s16(src[i]) * g) >> 16) << 1) & 0xffff;
      sInt d = dst[i] - m;
      dst[i] = (sU16)(d < 0 ? 0 : d);
    }
  }

  // dst = 2 * ((src * dst) >> 16), per channel (pmulhw, psllw)
  static void multiply(sU16 *dst, const sU16 *src, sInt cnt)
  {
    for (sInt i = 0; i < cnt * 4; i++)
      dst[i] = (sU16)(((s16(src[i]) * s16(dst[i])) >> 16) << 1);
  }

  static inline sS32 imul16(sS32 a, sS32 b) { return (sS32)(((long long)a * b) >> 16); }

  static inline sU32 srandom2(sU32 n)
  {
    n ^= n << 13;
    sU32 t = n * n * 15731u + 789221u;
    n = n * t + 1376312589u;
    return 2048u - ((n & 0x7fffffffu) >> 19);
  }

  // Crystal: per pixel the squared distance to the nearest of cnt random
  // points on the torus, then mapped from col1 to col2.
  static void crystal(sU16 *dptr, sInt width, sInt height, sU32 sed, sU32 cnt, sU32 col1, sU32 col2)
  {
    sU8 randBuf[512 + 4];
    sU32 edx = sed;
    for (sInt i = 511; i >= 0; i--)
    {
      edx = edx * 0x015a4e35u + 1;
      randBuf[i] = (sU8)(edx >> 16);
    }
    randBuf[512] = randBuf[513] = randBuf[514] = randBuf[515] = 0;

    const sInt hw = (width >> 1) & 0x7fff, hh = (height >> 1) & 0x7fff;
    sU32 maxd = 0;
    sU32 *dist = (sU32 *)dptr; // the asm keeps the distance in the first two channels
    for (sInt y = 0; y < height; y++)
      for (sInt x = 0; x < width; x++)
      {
        sS32 best = 0x7fffffff;
        for (sInt e = (sInt)cnt - 1; e >= 0; e--)
        {
          sInt px = (randBuf[e * 2] | (randBuf[e * 2 + 1] << 8)) & (width - 1);
          sInt py = (randBuf[e * 2 + 2] | (randBuf[e * 2 + 3] << 8)) & (height - 1);
          sInt dx = s16(x - px), dy = s16(y - py);
          dx = sat16(dx < 0 ? -dx : dx);
          dy = sat16(dy < 0 ? -dy : dy);
          if (dx > hw) dx = s16(width - dx);
          if (dy > hh) dy = s16(height - dy);
          sS32 d = dx * dx + dy * dy;
          if (best > d) best = d;
        }
        if ((sU32)best > maxd) maxd = best;
        dist[(y * width + x) * 2] = best;
      }

    sInt c0[4], dc[4];
    for (sInt i = 0; i < 4; i++)
    {
      sInt a = ((col1 >> (i * 8)) & 0xff) << 7, b = ((col2 >> (i * 8)) & 0xff) << 7;
      c0[i] = a;
      dc[i] = s16(b - a);
    }
    for (sInt p = width * height - 1; p >= 0; p--)
    {
      sS32 d = (sS32)dist[p * 2];
      sS32 q = (sS32)(((long long)d * 65536) / (sS32)maxd);
      sS32 a = (sS32)(~(sU32)q + 0xffffu);
      sS32 sq = (sS32)(((long long)a * a) >> 16);
      sU32 b = ~(sU32)sq + 0xffffu;
      sU32 t = b >> 1;
      sU16 *px = dptr + p * 4;
      for (sInt i = 0; i < 4; i++)
        px[i] = (sU16)(c0[i] + (((s16(t) * dc[i]) >> 16) << 1));
      px[3] = (sU16)t;
    }
  }

  // One row of the box blur (inplaceBlur): src is the row padded with radius
  // pixels on each side (pbuf + radius * 4); writes w pixels to dst.
  static void blurRow(const sU16 *pbuf, const sU16 *src, sU16 *dst, sU32 w, sU32 radius, sU32 sc)
  {
    sU32 sum[4] = {0, 0, 0, 0};
    for (sU32 i = 0; i <= 2 * radius; i++)
      for (sInt c = 0; c < 4; c++)
        sum[c] += pbuf[i * 4 + c];
    const sInt scw = s16(sc);
    for (sU32 x = 0; x < w; x++)
    {
      for (sInt c = 0; c < 4; c++)
      {
        sU32 s = sum[c];
        sU32 hi = ((s >> 16) * (sU32)(sc & 0xffff)) & 0xffff;               // pmullw on the high word
        sInt lo = ((s16((s & 0xffff) >> 1) * scw) >> 16) & 0xffff;          // pmulhw on (low word >> 1)
        sS32 v = (sS32)(hi + ((sU32)lo << 1));
        dst[x * 4 + c] = (sU16)sat16(v);
      }
      const sU16 *add = src + (x + radius + 1) * 4;
      const sU16 *sub = src + ((sInt)x - (sInt)radius) * 4;
      for (sInt c = 0; c < 4; c++)
        sum[c] += (sU32)add[c] - (sU32)sub[c];
    }
  }
}

#endif
