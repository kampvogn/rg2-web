// kampvogn.dk: debug.cpp for the browser: messages to the console, errors end the program.
#include "types.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include "debug.h"

namespace fr
{
  static debug::reportErrorProc reportError = 0;

  void __cdecl errorExit(const sChar *format, ...)
  {
    char buf[2048];
    va_list ap; va_start(ap, format); vsnprintf(buf, sizeof buf, format, ap); va_end(ap);
    if (reportError) reportError(buf);
    fprintf(stderr, "error: %s\n", buf);
    exit(1);
  }

  void __cdecl debugOut(const sChar *format, ...)
  {
    va_list ap; va_start(ap, format); vfprintf(stderr, format, ap); va_end(ap);
  }

  void atExit(debug::exitProc exitProc) { atexit(exitProc); }
  void setReportError(debug::reportErrorProc error) { reportError = error; }
}
