// kampvogn.dk: headless RG2 exporter. Loads an .rg21 or .tg2 project and
// writes export.dat (the player's introData) into the current directory,
// as the editor's "Make Demo" does before it links the viewer executable.
#include "stdafx.h"
#include "frOpGraph.h"
#include "exporter.h"
#include "texbase.h"
#include "tstream.h"

extern frOpGraph *g_graph;
extern void frInitTextureBase();

int main(int argc, char **argv)
{
  if (argc < 2) { fprintf(stderr, "usage: rg2export project.rg21|project.tg2 [xres yres]\n"); return 1; }
  const char *name = argv[1];
  sInt xRes = argc > 3 ? atoi(argv[2]) : 0, yRes = argc > 3 ? atoi(argv[3]) : 0;

  frInitTextureBase();
  g_graph = new frOpGraph;

  fr::streamF f;
  if (!f.open(name)) { fprintf(stderr, "cannot open %s\n", name); return 1; }
  fr::streamWrapper strm(f, sFALSE);
  sInt l = strlen(name);
  if (l >= 4 && !strcasecmp(name + l - 4, ".tg2")) g_graph->import(strm);
  else g_graph->serialize(strm);
  f.close();
  fprintf(stderr, "loaded %s: %d ops\n", name, (int)g_graph->m_ops.size());

  frGraphExporter exporter;
  exporter.makeDemo(xRes, yRes);
  return 0;
}
