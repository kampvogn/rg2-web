// MSVC-isms for clang/Emscripten (kampvogn.dk Candytron port)
#pragma once
#define __forceinline inline __attribute__((always_inline))
#define __assume(x) ((void)0)
#define __int64 long long
#define _cdecl
#define __cdecl
#define __stdcall
#define __fastcall
