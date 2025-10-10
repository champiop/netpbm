#include "netpbm.h"
#include <assert.h>
#include <stdio.h>

int main(void) {
  // Creates a valid bitmap image
  netpbm_image_t *image = netpbm_create(10, 10, BIT, 1);

  assert(image != NULL);
  assert(image->w == 10);
  assert(image->h == 10);
  assert(image->mode == BIT);
  netpbm_destroy(image);

  // Creates a valid grayscale image
  image = netpbm_create(800, 600, GRAY, 255);

  assert(image != NULL);
  assert(image->w == 800);
  assert(image->h == 600);
  assert(image->mode == GRAY);
  netpbm_destroy(image);

  // Creates a valid rgb image
  image = netpbm_create(10, 12, RGB, 255);

  assert(image != NULL);
  assert(image->w == 10);
  assert(image->h == 12);
  assert(image->mode == RGB);
  netpbm_destroy(image);

  return 0;
}
