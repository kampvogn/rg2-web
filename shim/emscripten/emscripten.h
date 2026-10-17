#pragma once
#define EMSCRIPTEN_KEEPALIVE
#define EM_JS(r,n,a,...) r n a { return r(); }
