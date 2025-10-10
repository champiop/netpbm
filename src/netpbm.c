#include "netpbm.h"

#include <errno.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

netpbm_image_t *netpbm_create(size_t w, size_t h, netpbm_mode_t mode,
                              uint16_t max_val) {
  if (mode != BIT && mode != GRAY && mode != RGB) {
    printf("Error: Unknow image mode\n");
    return NULL;
  }

  if (max_val == 0) {
    printf("Error: Max value must be greater than 0\n");
    return NULL;
  }

  if (mode == BIT && max_val != 1) {
    printf("Warning: For image mode BIT max value should be 1\n"
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

  if (mode == RGB && w * h > SIZE_MAX / 3) {
    printf("Error: Image is too big\n");
    free(image);
    return NULL;
  }

  image->data = malloc(w * h * (mode == RGB ? 3 : 1) * sizeof(uint16_t));
  if (image->data == NULL) {
    printf("Error: Could not allocate memory for image data\n");
    free(image);
    return NULL;
  }

  image->w = w;
  image->h = h;
  image->mode = mode;
  image->max_val = max_val;
  memset(image->data, 0, w * h * (mode == RGB ? 3 : 1) * sizeof(uint16_t));

  return image;
}

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
  case BIT:
    fputc('4', f);
    break;
  case GRAY:
    fputc('5', f);
    break;
  case RGB:
    fputc('6', f);
    break;
  default:
    printf("Error: Unknown image mode\n");
    return -1;
  }

  fputc('\n', f);
  fprintf(f, "%lu %lu\n", image->w, image->h);
  fprintf(f, "%d\n", image->max_val);

  size_t total = image->w * image->h * (image->mode == RGB ? 3 : 1);
  for (size_t i = 0; i < total; i++) {
    fputc((char)image->data[i], f);
  }

  fclose(f);

  return 0;
}

int netpbm_togray(netpbm_image_t *image) {
  switch (image->mode) {
  case BIT:
    image->max_val = 255;
    for (size_t i = 0; i < image->w * image->h; i++) {
      image->data[i] *= image->max_val;
    }
    image->mode = GRAY;
    break;
  case GRAY:
    break;
  case RGB:
    for (size_t i = 0; i < image->w * image->h; i++) {
      image->data[i] = (image->data[3 * i] + image->data[3 * i + 1] +
                        image->data[3 * i + 2]) /
                       3;
    }
    image->mode = GRAY;
    break;
  default:
    printf("Error: Unknown image mode when trying to convert to gray mode\n");
    return -1;
  }

  return 0;
}

int netpbm_torgb(netpbm_image_t *image) {
  switch (image->mode) {
  case BIT:
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
    image->mode = RGB;

    break;
  case GRAY:
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
    image->mode = RGB;

    break;
  case RGB:
    break;
  default:
    printf("Error: Unknown image mode when trying to convert to gray mode\n");
    return -1;
  }

  return 0;
}

void netpbm_destroy(netpbm_image_t *image) {
  free(image->data);
  free(image);
}
