#ifndef NETPBM_H
#define NETPBM_H

#include <stdint.h>
#include <stdlib.h>

// Specifies what type of netpbm image is handled
// BIT      =>    Portable Bit  Map
// GRAY     =>    Portable Gray Map
// RGB      =>    Portable Pix  Map
typedef enum {
  UNKNOWN = 0,
  BIT,
  GRAY,
  RGB,
} netpbm_mode_t;

// Holds the data relevant to an image
// w        =>    Image width
// h        =>    Image height
// mode     =>    Bit, Gray or RGB
// max_val  =>    Maximum sample value
// data     =>    Data samples stored channel first, width second and height
//                last
typedef struct netpbm_image {
  size_t w;
  size_t h;
  netpbm_mode_t mode;
  uint16_t max_val;
  uint16_t *data;
} netpbm_image_t;

// Creates an image of size w x h with indicated color mode and max sample value
// Returns NULL on error, or the image on success
netpbm_image_t *netpbm_create(size_t w, size_t h, netpbm_mode_t mode,
                              uint16_t max_val);

// Opens an existing netpbm image saved under filename
// Returns NULL on error, or the image on success
netpbm_image_t *netpbm_open(const char *filename);

// Saves the given image under filename
// Returns -1 on error, 0 on success
int netpbm_save(netpbm_image_t *image, const char *filename);

// Frees data allocated by image
void netpbm_destroy(netpbm_image_t *image);

#endif // NETPBM_H
