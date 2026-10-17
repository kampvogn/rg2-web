// kampvogn.dk: loader.asm (incbin "tpinv.v2m") for the headless exporter
__asm__(".data\n"
        ".globl loaderTune\nloaderTune:\n.incbin \"ViruzII/tpinv.v2m\"\n"
        ".globl loaderTuneEnd\nloaderTuneEnd:\n.byte 0\n"
        ".text\n");
