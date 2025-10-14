#include "netpbm.h"

#include <linux/limits.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Holds the data relevant to an image
// w        =>    Image width
// h        =>    Image height
// mode     =>    Bit, Gray or RGB
// max_val  =>    Maximum sample value
// data     =>    Data samples stored channel first, width second and height
//                last
struct netpbm_image {
  size_t w;
  size_t h;
  netpbm_mode_t mode;
  uint16_t max_val;
  uint16_t *data;
};

// Resource management

netpbm_image_t *netpbm_create(size_t w, size_t h, netpbm_mode_t mode,
                              uint16_t max_val) {
  if (mode != NETPBM_MODE_BIT && mode != NETPBM_MODE_GRAY &&
      mode != NETPBM_MODE_RGB) {
    printf("Error: Unknow image mode\n");
    return NULL;
  }

  if (max_val == 0) {
    printf("Error: Max value must be greater than 0\n");
    return NULL;
  }

  if (mode == NETPBM_MODE_BIT && max_val != 1) {
    printf("Warning: For image mode NETPBM_MODE_BIT max value should be 1\n"
           "         Ignoring given value\n");
    max_val = 1;
  }

  netpbm_image_t *image = malloc(sizeof(netpbm_image_t));
  if (image == NULL) {
    printf("Error: Could not allocate memory for image\n");
    return NULL;
  }

  if (h != 0 && w > SIZE_MAX / h) {
    printf("Error: Image is too big\n");
    free(image);
    return NULL;
  }

  if (mode == NETPBM_MODE_RGB && w * h > SIZE_MAX / 3) {
    printf("Error: Image is too big\n");
    free(image);
    return NULL;
  }

  image->data =
      malloc(w * h * (mode == NETPBM_MODE_RGB ? 3 : 1) * sizeof(uint16_t));
  if (image->data == NULL) {
    printf("Error: Could not allocate memory for image data\n");
    free(image);
    return NULL;
  }

  image->w = w;
  image->h = h;
  image->mode = mode;
  image->max_val = max_val;
  memset(image->data, 0,
         w * h * (mode == NETPBM_MODE_RGB ? 3 : 1) * sizeof(uint16_t));

  return image;
}

void netpbm_destroy(netpbm_image_t *image) {
  free(image->data);
  free(image);
}

// File handling

int netpbm_save(netpbm_image_t *image, const char *filename) {
  if (filename == NULL) {
    printf("Error: Could not open output file\n");
    return -1;
  }

  FILE *f = fopen(filename, "w");
  if (f == NULL) {
    printf("Error: Could not open output file\n");
    return -1;
  }

  fputc('P', f);
  switch (image->mode) {
  case NETPBM_MODE_BIT:
    fputc('4', f);
    break;
  case NETPBM_MODE_GRAY:
    fputc('5', f);
    break;
  case NETPBM_MODE_RGB:
    fputc('6', f);
    break;
  default:
    printf("Error: Unknown image mode\n");
    return -1;
  }

  fputc('\n', f);
  fprintf(f, "%lu %lu\n", image->w, image->h);
  fprintf(f, "%d\n", image->max_val);

  size_t total = image->w * image->h * (image->mode == NETPBM_MODE_RGB ? 3 : 1);
  if (image->max_val < 256) {
    for (size_t i = 0; i < total; i++) {
      fputc((uint8_t)image->data[i], f);
    }
  } else {
    for (size_t i = 0; i < total; i++) {
      fputc((uint8_t)(image->data[i] >> 8), f);
      fputc((uint8_t)(image->data[i] & 0x00ff), f);
    }
  }

  fclose(f);

  return 0;
}

// Accessors

size_t netpbm_get_width(netpbm_image_t *image) { return image->w; }
size_t netpbm_get_height(netpbm_image_t *image) { return image->h; }
netpbm_mode_t netpbm_get_mode(netpbm_image_t *image) { return image->mode; }
uint16_t netpbm_get_max_val(netpbm_image_t *image) { return image->max_val; }

uint16_t netpbm_get_raw(netpbm_image_t *image, size_t idx) {
  if (idx >= image->w * image->h * (image->mode == NETPBM_MODE_RGB ? 3 : 1)) {
    printf("Error: Index out of bounds");
    return 0;
  }

  return image->data[idx];
}

uint16_t netpbm_get(netpbm_image_t *image, size_t x, size_t y,
                    netpbm_channel_t c) {
  switch (c) {
  case NETPBM_CHANNEL_GRAY:
    if (image->mode != NETPBM_MODE_BIT && image->mode != NETPBM_MODE_GRAY) {
      printf("Error: Can't get value for channel GRAY with RGB mode image\n");
      return 0;
    }
    break;
  case NETPBM_CHANNEL_R:
    if (image->mode != NETPBM_MODE_RGB) {
      printf("Error: Can't get value for channel R with BIT/GRAY mode image\n");
      return 0;
    }
    break;
  case NETPBM_CHANNEL_G:
    if (image->mode != NETPBM_MODE_RGB) {
      printf("Error: Can't get value for channel G with BIT/GRAY mode image\n");
      return 0;
    }
    break;
  case NETPBM_CHANNEL_B:
    if (image->mode != NETPBM_MODE_RGB) {
      printf("Error: Can't get value for channel B with BIT/GRAY mode image\n");
      return 0;
    }
    break;
  default:
    printf("Error: Unknown channel\n");
    return 0;
  }

  if (x >= image->w || y >= image->h) {
    printf("Error: Coordinates out of image's bounds");
    return 0;
  }

  if (c == NETPBM_CHANNEL_GRAY) {
    return image->data[y * image->w + x];
  } else {
    return image->data[c * (image->w * image->h) + y * image->w + x];
  }
}

// Modifiers

int netpbm_set_raw(netpbm_image_t *image, size_t idx, uint16_t value) {
  if (idx >= image->w * image->h * (image->mode == NETPBM_MODE_RGB ? 3 : 1)) {
    printf("Error: Index out of bounds");
    return -1;
  }

  if (value > image->max_val) {
    printf("Warning: Given data value greater than maximum sample value, value "
           "set to max");
    value = image->max_val;
  }

  image->data[idx] = value;

  return 0;
}

int netpbm_set(netpbm_image_t *image, size_t x, size_t y, netpbm_channel_t c,
               uint16_t value) {
  switch (c) {
  case NETPBM_CHANNEL_GRAY:
    if (image->mode != NETPBM_MODE_BIT && image->mode != NETPBM_MODE_GRAY) {
      printf("Error: Can't set value for channel GRAY with RGB mode image\n");
      return -1;
    }
    break;
  case NETPBM_CHANNEL_R:
    if (image->mode != NETPBM_MODE_RGB) {
      printf("Error: Can't set value for channel R with BIT/GRAY mode image\n");
      return -1;
    }
    break;
  case NETPBM_CHANNEL_G:
    if (image->mode != NETPBM_MODE_RGB) {
      printf("Error: Can't set value for channel G with BIT/GRAY mode image\n");
      return -1;
    }
    break;
  case NETPBM_CHANNEL_B:
    if (image->mode != NETPBM_MODE_RGB) {
      printf("Error: Can't set value for channel B with BIT/GRAY mode image\n");
      return -1;
    }
    break;
  default:
    printf("Error: Unknown channel\n");
    return -1;
  }

  if (x >= image->w || y >= image->h) {
    printf("Error: Coordinates out of image's bounds");
    return -1;
  }

  if (value > image->max_val) {
    printf("Warning: Given data value greater than maximum sample value, value "
           "set to max");
    value = image->max_val;
  }

  if (c == NETPBM_CHANNEL_GRAY) {
    image->data[y * image->w + x] = value;
  } else {
    image->data[c * (image->w * image->h) + y * image->w + x] = value;
  }

  return 0;
}

int netpbm_togray(netpbm_image_t *image) {
  switch (image->mode) {
  case NETPBM_MODE_BIT:
    image->max_val = 255;
    for (size_t i = 0; i < image->w * image->h; i++) {
      image->data[i] *= image->max_val;
    }
    image->mode = NETPBM_MODE_GRAY;
    break;
  case NETPBM_MODE_GRAY:
    break;
  case NETPBM_MODE_RGB:
    for (size_t i = 0; i < image->w * image->h; i++) {
      image->data[i] = (image->data[3 * i] + image->data[3 * i + 1] +
                        image->data[3 * i + 2]) /
                       3;
    }
    image->mode = NETPBM_MODE_GRAY;
    break;
  default:
    printf("Error: Unknown image mode when trying to convert to gray mode\n");
    return -1;
  }

  return 0;
}

int netpbm_torgb(netpbm_image_t *image) {
  switch (image->mode) {
  case NETPBM_MODE_BIT:
    if (image->w * image->h > SIZE_MAX / 3) {
      printf("Error: Image is too big\n");
      return -1;
    }

    image->data =
        realloc(image->data, image->w * image->h * 3 * sizeof(uint16_t));
    if (image->data == NULL) {
      printf("Error: Could not allocate memory for image data\n");
      return -1;
    }

    image->max_val = 255;
    for (size_t i = image->w * image->h; i > 0; i--) {
      image->data[3 * (i - 1) + 0] = image->data[i] * image->max_val;
      image->data[3 * (i - 1) + 1] = image->data[i] * image->max_val;
      image->data[3 * (i - 1) + 2] = image->data[i] * image->max_val;
    }
    image->mode = NETPBM_MODE_RGB;

    break;
  case NETPBM_MODE_GRAY:
    if (image->w * image->h > SIZE_MAX / 3) {
      printf("Error: Image is too big\n");
      return -1;
    }

    image->data =
        realloc(image->data, image->w * image->h * 3 * sizeof(uint16_t));
    if (image->data == NULL) {
      printf("Error: Could not allocate memory for image data\n");
      return -1;
    }

    for (size_t i = image->w * image->h; i > 0; i--) {
      image->data[3 * (i - 1) + 0] = image->data[i];
      image->data[3 * (i - 1) + 1] = image->data[i];
      image->data[3 * (i - 1) + 2] = image->data[i];
    }
    image->mode = NETPBM_MODE_RGB;

    break;
  case NETPBM_MODE_RGB:
    break;
  default:
    printf("Error: Unknown image mode when trying to convert to gray mode\n");
    return -1;
  }

  return 0;
}
