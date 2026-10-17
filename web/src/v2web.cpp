// kampvogn.dk: V2 for the RG2 web player, on fr_public's v2/ (BSD).
//
// The RG2 intros were exported with RG2's ViruzII (V2 patch format version 5);
// fr_public's C++ synth (synth_core.cpp, compiled in v2synth.cpp) and its
// player (v2mplayer.cpp) want the newest version (6). The song is converted
// with V2's own ConvertV2M when it is loaded. Only V2's headers here (its
// types clash with RG2's); sound_web.cpp uses the three C functions below.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "v2mplayer.cpp"
#include "v2mconv.h"
#include "sounddef.h"

// v2mconv.cpp needs these from sounddef.cpp's sdInit() (which also does Win32
// file handling); built the same way here
int v2version;
int *v2vsizes;
int *v2gsizes;
static void InitVersions()
{
  v2version = 0;
  for (int i = 0; i < v2nparms; i++) if (v2parms[i].version > v2version) v2version = v2parms[i].version;
  for (int i = 0; i < v2ngparms; i++) if (v2gparms[i].version > v2version) v2version = v2gparms[i].version;
  v2vsizes = new int[v2version + 1]; v2gsizes = new int[v2version + 1];
  memset(v2vsizes, 0, (v2version + 1) * sizeof(int)); memset(v2gsizes, 0, (v2version + 1) * sizeof(int));
  for (int i = 0; i < v2nparms; i++) for (int j = v2version; j >= v2parms[i].version; j--) v2vsizes[j]++;
  for (int i = 0; i < v2ngparms; i++) for (int j = v2version; j >= v2gparms[i].version; j--) v2gsizes[j]++;
  for (int i = 0; i <= v2version; i++) v2vsizes[i] += 1 + 255 * 3;
}

static V2MPlayer Player;
static unsigned char *Song;

extern "C" void __stdcall OutputDebugStringA(const char *) {}

// v2m: the song as exported (any older V2M version); len: an upper bound of its size
extern "C" int v2web_init(const void *v2m, int len)
{
  if (!v2version) InitVersions();
  int outlen = 0;
  if (Song) delete[] Song;
  Song = 0;
  int delta = CheckV2MVersion((const unsigned char *)v2m, len);
  if (delta < 0) { printf("V2M not recognised (%d)\n", delta); return 0; }
  ConvertV2M((const unsigned char *)v2m, len, &Song, &outlen);
  if (!Song) return 0;
  Player.Init();
  return Player.Open(Song, 44100) ? 1 : 0;
}
extern "C" void v2web_play(int ms) { Player.Play(ms); }
extern "C" void v2web_stop() { Player.Stop(); }
extern "C" void v2web_render(float *buf, int samples) { Player.Render(buf, samples); }
