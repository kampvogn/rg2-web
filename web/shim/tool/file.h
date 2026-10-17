// kampvogn.dk: a stand-in for fr_public's tool/File.h (Win32 file handles).
// tools/convv2m only needs sounddef.cpp's parameter tables, never its bank
// loading/saving, so these classes just compile and do nothing.
#pragma once
#include "types.h"
class file
{
public:
  virtual ~file() {}
  virtual sS32 read(void *, sS32) { return 0; }
  virtual sS32 write(const void *, sS32) { return 0; }
  virtual sBool eof() { return 1; }
  virtual sS32 seekcur(sS32) { return 0; }
  virtual sS32 puts(const char *) { return 0; }
  sU8 getsU8() { return 0; }
  sU16 getsU16() { return 0; }
  sU32 getsU32() { return 0; }
  sS32 putsU8(sU8) { return 0; }
  sS32 putsU16(sU16) { return 0; }
  sS32 putsU32(sU32) { return 0; }
};
class fileM : public file
{
public:
  sBool open(const void *, sS32) { return 0; }
  sBool openNew(sS32 = 0) { return 0; }
  sBool close() { return 1; }
  void *detach() { return 0; }
};
