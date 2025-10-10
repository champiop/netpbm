#include "netpbm.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
  netpbm_image_t *image = netpbm_create(10, 10, GRAY, 255);

  assert(image != NULL);
  assert(image->w == 10);
  assert(image->h == 10);
  assert(image->mode == GRAY);
  netpbm_destroy(image);

  return 0;
}
