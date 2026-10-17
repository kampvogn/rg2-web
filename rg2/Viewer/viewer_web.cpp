// kampvogn.dk: the RG2 viewer's main program for the browser (Viewer.cpp is
// the Win32 one). The page puts the exported intro (export.dat) at
// /intro.dat in Emscripten's file system before it calls main(). The steps
// are Viewer.cpp's: precalculation with a progress bar, then the operators
// are animated against the music's clock until the intro ends. Frames and
// the progress bar wait for the browser's next animation frame (Asyncify).
#ifdef __EMSCRIPTEN__
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <emscripten.h>
#include "types.h"
#include "directx.h"
#include "rtlib.h"
#include "soundsys.h"
#include "engine.h"
#include "debug.h"
#include "opsys.h"
#include "cmpop.h"

extern "C" const sU8 *web_data_end = 0;

// the viewer relies on new returning zeroed memory (its own new used HEAP_ZERO_MEMORY)
void *operator new(size_t sz) { return calloc(1, sz ? sz : 1); }
void *operator new[](size_t sz) { return calloc(1, sz ? sz : 1); }
void operator delete(void *p) noexcept { free(p); }
void operator delete[](void *p) noexcept { free(p); }
void operator delete(void *p, size_t) noexcept { free(p); }
void operator delete[](void *p, size_t) noexcept { free(p); }

EM_ASYNC_JS(void, web_wait_frame, (), {
  await new Promise((r) => requestAnimationFrame(() => r()));
});
EM_JS(int, web_quit, (), { return Module.webQuit ? 1 : 0; });
EM_JS(void, web_progress, (double f), { if (Module.onProgress) Module.onProgress(f); });
EM_JS(void, web_end, (const char *why), { if (Module.onDemoEnd) Module.onDemoEnd(UTF8ToString(why)); });
EM_JS(int, web_start_ms, (), { return Module.startMs | 0; });

extern "C" void ssSetStart(sS32 ms);

static void __stdcall tgPoll(sInt cnt, sInt max)
{
  static double oldt = 0;
  const double curt = emscripten_get_now();

  if (curt - oldt >= 100)
  {
    oldt = curt;

    fvs::clear();

    fvs::vbSlice vbs(4, sTRUE);
    fvs::stdVertex* v = (fvs::stdVertex*) vbs.lock();

    for (sInt i = 0; i < 4; i++)
    {
      v[i].x = (0.0625000f + (i & 1) * cnt * 0.875f / max) * fvs::conf.xRes;
      v[i].y = (0.4583333f + (i & 2) * 0.041667f) * fvs::conf.yRes;
      v[i].z = v[i].rhw = 0.5f;
      v[i].color = 0xffffff;
    }

    vbs.unlock();
    fvs::drawPrimitive(D3DPT_TRIANGLESTRIP, vbs.use(), 2);
    fvs::updateScreen();
    web_progress(max ? (double) cnt / max : 0);
    web_wait_frame();
  }
}

static void __stdcall errorReport(const sChar *msg)
{
  printf("error: %s\n", msg);
  web_end("abort");
  emscripten_force_exit(1);
}

int main()
{
  FILE *f = fopen("/intro.dat", "rb");
  if (!f) { printf("no /intro.dat\n"); web_end("abort"); return 1; }
  fseek(f, 0, SEEK_END);
  long n = ftell(f);
  fseek(f, 0, SEEK_SET);
  sU8 *data = (sU8 *) calloc(1, n + 16);
  fread(data, 1, n, f);
  fclose(f);
  web_data_end = data + n;

  const sU8* dataPtr = data;
  sInt xRes = ((const sInt*) dataPtr)[0];
  sInt yRes = ((const sInt*) dataPtr)[1];
  const sU8* loaderTune = ((const sU8**) dataPtr)[2]; // relocated by Make Demo; 0 in these exports
  dataPtr += sizeof(sInt) * 3;

  if (!xRes || !yRes) { xRes = 800; yRes = 600; } // "default" in the editor's resolution chooser

  fvs::conf.xRes = xRes;
  fvs::conf.yRes = yRes;
  fvs::conf.triplebuffer = sTRUE;

  fr::setReportError(errorReport);
  fvs::startupDirectX(0);
  engineInit();

  if (loaderTune)
  {
    ssInit((sU8 *) data + ((sU32) loaderTune), 0);
    ssPlay();
  }

  // perform precalc
  const sU8* viruzPtr = opsInitialize(dataPtr);
  opsCalculate(tgPoll);
  web_progress(1);

  if (loaderTune)
    ssClose();

  // Module.startMs (the page's ?t=) starts somewhere in the middle: the
  // timeline is run up to there once, and the music plays from there
  const sInt startMs = web_start_ms();
  ssInit((sU8*) viruzPtr, 0);
  ssSetStart(startMs);
  ssPlay();
  while (ssGetTime() < 0)
    web_wait_frame();

  sInt oldTime = 0;
  if (startMs > 0)
  {
    opsAnimate(-1, 0, startMs / 8);
    oldTime = startMs / 8;
  }
  while (!web_quit())
  {
    sInt time = ssGetTime() / 8; // exported timing is in 1/125s, not ms.

    if (!opsAnimate(-1, oldTime, time))
      break;

    cmpPaintFrame();
    ssDoTick();
    fvs::updateScreen();
    web_wait_frame();

    oldTime = time;
  }

  ssStop();
  web_end("end");
  return 0;
}
#endif
