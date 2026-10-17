// This file is distributed under a BSD license. See LICENSE.txt for details.
// kampvogn.dk: fr_public's C++ port of the V2 synth (see v2web.cpp).
#include <stdio.h>
#define vsprintf_s(buf, fmt, args) vsnprintf(buf, sizeof(buf), fmt, args) // MSVC array overload
#include "synth_core.cpp"
