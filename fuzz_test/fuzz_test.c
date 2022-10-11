#include "qoi/qoi.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

extern void LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  qoi_desc_t desc = {0};
  void *pixels = qoi_decode((void const *)data, (uint64_t)size, &desc);
  if (pixels) {
    free(pixels);
  }
}
