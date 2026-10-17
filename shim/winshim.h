// Minimal Win32 surface for building RG2's non-GUI code (load + export) with
// Emscripten and running it in node. Single-threaded: the synchronisation
// objects do nothing, files are stdio FILE*s.
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "d3d9.h"      // d3d9-webgl: Windows base types and the D3D9 declarations

#define __forceinline inline __attribute__((always_inline))
#ifndef __stdcall
#define __stdcall
#endif
#ifndef __cdecl
#define __cdecl
#endif
#ifndef _T
#define _T(x) x
#endif

#ifndef _HANDLE_DEFINED
#define _HANDLE_DEFINED
typedef void *HANDLE;
#endif
typedef char TCHAR;
typedef void *HMONITOR, *HDC, *HBITMAP, *HFONT, *HGDIOBJ;
typedef RECT *LPRECT;
#define stricmp strcasecmp
#define lstrcpy strcpy
#define lstrcat strcat
#define lstrlen strlen
#define wsprintf sprintf
#define ATLASSERT(x) ((void)0)
#define WM_USER 0x400
#define TRANSPARENT 1
#define ANTIALIASED_QUALITY 4
#define MB_ICONQUESTION 0x20
#define MB_YESNO 4
#define IDYES 6
#define _P_WAIT 0
#define _P_NOWAIT 1
static inline int spawnlp(int, const char *, ...) { return -1; }
static inline int spawnl(int, const char *, ...) { return -1; }
static inline DWORD GetModuleFileName(HMODULE, char *buf, DWORD n) { if (n) buf[0] = 0; return 0; }
static inline HWND GetForegroundWindow() { return 0; }
static inline intptr_t SendMessage(HWND, unsigned, uintptr_t, intptr_t) { return 0; }
struct CRect : RECT { CRect() { left = top = right = bottom = 0; } CRect(const RECT &r) { *(RECT *)this = r; } };
typedef struct { int abcA; unsigned abcB; int abcC; } ABC;
typedef void *HBRUSH;
// GDI is only used by the text operator, which the exporter never runs
static inline HFONT CreateFont(int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, const char *) { return 0; }
static inline HBITMAP CreateDIBSection(HDC, const void *, unsigned, void **bits, HANDLE, DWORD) { if (bits) *bits = 0; return 0; }
static inline HBRUSH CreateSolidBrush(unsigned) { return 0; }
static inline int FillRect(HDC, const RECT *, HBRUSH) { return 1; }
static inline int AddFontResource(const char *) { return 1; }
static inline BOOL GetCharABCWidths(HDC, unsigned, unsigned, ABC *abc) { abc->abcA = abc->abcC = 0; abc->abcB = 0; return 1; }
static inline HDC GetDC(HWND) { return 0; }
static inline int ReleaseDC(HWND, HDC) { return 0; }
static inline HWND GetDesktopWindow() { return 0; }
static inline HDC CreateCompatibleDC(HDC) { return 0; }
static inline BOOL DeleteDC(HDC) { return 1; }
static inline HGDIOBJ SelectObject(HDC, HGDIOBJ) { return 0; }
static inline BOOL DeleteObject(HGDIOBJ) { return 1; }
static inline unsigned SetTextColor(HDC, unsigned) { return 0; }
static inline int SetBkMode(HDC, int) { return 0; }
static inline BOOL TextOut(HDC, int, int, const char *, int) { return 1; }
static inline BOOL GdiFlush() { return 1; }
static inline BOOL RemoveFontResource(const char *) { return 1; }
static inline BOOL PostMessage(HWND, unsigned, uintptr_t, intptr_t) { return 1; }
typedef void *HGLOBAL, *HRSRC;
typedef struct { int dummy; } CRITICAL_SECTION;

#ifndef INVALID_HANDLE_VALUE
#define INVALID_HANDLE_VALUE ((HANDLE)(intptr_t)-1)
#endif
#define INFINITE 0xFFFFFFFF
#define WAIT_OBJECT_0 0
#define WAIT_TIMEOUT 258
#define DUPLICATE_SAME_ACCESS 2
#define GENERIC_READ 0x80000000
#define GENERIC_WRITE 0x40000000
#define FILE_SHARE_READ 1
#define CREATE_ALWAYS 2
#define OPEN_EXISTING 3
#define OPEN_ALWAYS 4
#define FILE_BEGIN 0
#define FILE_CURRENT 1
#define FILE_END 2
#define RT_RCDATA ((const char *)10)
#define MAKEINTRESOURCE(i) ((const char *)(uintptr_t)(i))
#ifndef MB_OK
#define MB_OK 0
#endif
#ifndef MB_ICONERROR
#define MB_ICONERROR 0x10
#endif

static inline HANDLE GetCurrentProcess() { return (HANDLE)1; }
static inline BOOL DuplicateHandle(HANDLE, HANDLE s, HANDLE, HANDLE *d, DWORD, BOOL, DWORD) { *d = s; return 1; }
static inline BOOL CloseHandle(HANDLE h) { return 1; }
static inline HANDLE CreateMutex(void *, BOOL, const char *) { return (HANDLE)2; }
static inline BOOL ReleaseMutex(HANDLE) { return 1; }
static inline HANDLE CreateEvent(void *, BOOL, BOOL, const char *) { return (HANDLE)3; }
static inline BOOL SetEvent(HANDLE) { return 1; }
static inline BOOL ResetEvent(HANDLE) { return 1; }
static inline DWORD WaitForSingleObject(HANDLE, DWORD) { return WAIT_OBJECT_0; }
static inline DWORD WaitForMultipleObjects(DWORD, const HANDLE *, BOOL, DWORD) { return WAIT_OBJECT_0; }
static inline void InitializeCriticalSection(CRITICAL_SECTION *) {}
static inline void DeleteCriticalSection(CRITICAL_SECTION *) {}
static inline void EnterCriticalSection(CRITICAL_SECTION *) {}
static inline void LeaveCriticalSection(CRITICAL_SECTION *) {}
static inline BOOL TryEnterCriticalSection(CRITICAL_SECTION *) { return 1; }
static inline long InterlockedIncrement(volatile long *p) { return ++*p; }
static inline long InterlockedDecrement(volatile long *p) { return --*p; }
static inline HMODULE GetModuleHandle(const char *) { return 0; }
static inline HRSRC FindResource(HMODULE, const char *, const char *) { return 0; }
static inline HGLOBAL LoadResource(HMODULE, HRSRC) { return 0; }
static inline void *LockResource(HGLOBAL) { return 0; }
static inline DWORD SizeofResource(HMODULE, HRSRC) { return 0; }
static inline void OutputDebugString(const char *s) { fputs(s, stderr); }

// files on stdio (file handles are FILE*s; the code only compares them with INVALID_HANDLE_VALUE)
static inline HANDLE CreateFile(const char *name, DWORD access, DWORD, void *, DWORD disp, DWORD, HANDLE)
{
  const char *mode = (access & GENERIC_WRITE) ? (disp == OPEN_EXISTING || disp == OPEN_ALWAYS ? "r+b" : "w+b") : "rb";
  FILE *f = fopen(name, mode);
  if (!f && disp == OPEN_ALWAYS) f = fopen(name, "w+b");
  return f ? (HANDLE)f : INVALID_HANDLE_VALUE;
}
static inline BOOL ReadFile(HANDLE h, void *buf, DWORD n, DWORD *got, void *) { *got = (DWORD)fread(buf, 1, n, (FILE *)h); return 1; }
static inline BOOL WriteFile(HANDLE h, const void *buf, DWORD n, DWORD *put, void *) { *put = (DWORD)fwrite(buf, 1, n, (FILE *)h); return 1; }
static inline DWORD SetFilePointer(HANDLE h, LONG d, LONG *, DWORD how)
{
  fseek((FILE *)h, d, how == FILE_BEGIN ? SEEK_SET : how == FILE_CURRENT ? SEEK_CUR : SEEK_END);
  return (DWORD)ftell((FILE *)h);
}
static inline DWORD GetFileSize(HANDLE h, DWORD *)
{
  FILE *f = (FILE *)h; long p = ftell(f); fseek(f, 0, SEEK_END); long s = ftell(f); fseek(f, p, SEEK_SET); return (DWORD)s;
}
static inline BOOL FlushFileBuffers(HANDLE h) { fflush((FILE *)h); return 1; }
