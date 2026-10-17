// kampvogn.dk: the RG2 viewer's sound system (soundsys.cpp on the V2 asm
// synth, dsio.asm on DirectSound) for the browser: SDL2 audio calls back for
// samples, which V2MPlayer renders (v2web.cpp). ssGetTime(), the intro's
// clock, is the audio context's time since playback started, minus one
// buffer of latency, in milliseconds (TIMER_TICKS_PER_SEC).
#include <SDL2/SDL.h>
#include <emscripten.h>
#include "types.h"
#include "soundsys.h"

extern "C" int v2web_init(const void *v2m, int len);
extern "C" void v2web_play(int ms);
extern "C" void v2web_stop();
extern "C" void v2web_render(float *buf, int samples);

// the end of the intro data, as an upper bound for the song's length
extern "C" const sU8 *web_data_end;

#define WEBSAMPLES 2048
static SDL_AudioDeviceID dev;
static double startTime = -1;   // audio context time of the first callback after ssPlay()
static int playing = 0, ready = 0;
static sS32 startMs = 0;        // where in the song ssPlay() starts (ssSetStart)

EM_JS(double, web_audio_time, (), {
  const c = (typeof SDL2 !== 'undefined' && SDL2.audioContext) ? SDL2.audioContext : null;
  return c ? c.currentTime : performance.now() / 1000;
});

static void callback(void *, Uint8 *stream, int len)
{
  float *buf = (float *)stream;
  int n = len / 8;
  if (!playing || !ready) { memset(stream, 0, len); return; }
  if (startTime < 0) startTime = web_audio_time();
  v2web_render(buf, n);
}

extern "C"
{
  void __stdcall ssInit(sU8 *data, void *)
  {
    if (!dev)
    {
      SDL_AudioSpec want, have;
      SDL_InitSubSystem(SDL_INIT_AUDIO);
      memset(&want, 0, sizeof(want));
      want.freq = 44100;
      want.format = AUDIO_F32;
      want.channels = 2;
      want.samples = WEBSAMPLES;
      want.callback = callback;
      dev = SDL_OpenAudioDevice(0, 0, &want, &have, 0);
    }
    SDL_LockAudioDevice(dev);
    ready = v2web_init(data, (int)(web_data_end - data));
    playing = 0;
    startTime = -1;
    SDL_UnlockAudioDevice(dev);
    if (dev) SDL_PauseAudioDevice(dev, 0);
  }

  void __stdcall ssClose()
  {
    SDL_LockAudioDevice(dev);
    playing = ready = 0;
    SDL_UnlockAudioDevice(dev);
  }

  void __stdcall ssPlay()
  {
    SDL_LockAudioDevice(dev);
    v2web_play(startMs);
    startTime = -1;
    playing = 1;
    SDL_UnlockAudioDevice(dev);
  }

  void __stdcall ssStop()
  {
    SDL_LockAudioDevice(dev);
    v2web_stop();
    playing = 0;
    SDL_UnlockAudioDevice(dev);
  }

  sS32 __stdcall ssGetTime()
  {
    if (!playing || startTime < 0) return -1; // the viewer waits while this is negative
    double ms = (web_audio_time() - startTime) * 1000.0 - WEBSAMPLES * 1000.0 / 44100.0;
    return startMs + (ms < 0 ? 0 : (sS32)ms);
  }

  void ssSetStart(sS32 ms) { startMs = ms > 0 ? ms : 0; }

  void __stdcall ssFadeOut(sU32) {}
  void __stdcall ssDoTick() {}
}
