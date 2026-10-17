// This code is in the public domain. See LICENSE for details.

// runtime library header

#ifndef __rtlib_h_
#define __rtlib_h_

#define  PI    3.1415926535897932384626433832795f
#define  SQRT2 1.4142135623730950488016887242097f

#pragma warning (disable: 4244)

#include "math.h"
#include "string.h"

#pragma intrinsic (sin, cos, atan2, sqrt, memcpy, memset, fabs, pow, exp)

#ifdef __EMSCRIPTEN__
// kampvogn.dk: pow is Rtlib.asm's in the intros, which returns 0 for a base
// of 0 whatever the exponent (the C library says pow(0, 0) = 1) and NaN for
// a negative base (fyl2x). ein.schlag's glow gradients with exponent 0 rely
// on the 0: black outside the glow, not the glow's brightness everywhere.
static inline double rtlib_pow(double a, double b) { return a == 0.0 ? 0.0 : a < 0.0 ? NAN : pow(a, b); }
#define pow rtlib_pow
#endif

extern "C" size_t __cdecl   strlen(const char *s);

#endif
