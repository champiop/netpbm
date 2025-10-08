#include "netpbm.h"

#include <errno.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Displays netpbm utility man
void print_usage(const char *exe_name) {
  printf("netpbm utility\n"
         "Usage:\n"
         "  %s info   <filename_in>\n"
         "  %s load   <filename_in> save <filename_out>\n"
         "  %s create <filename_out> <w> <h> <mode>\n"
         "  (mode is 'bit', 'gray' or 'rgb')\n",
         exe_name, exe_name, exe_name);
}

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

void netpbm_destroy(netpbm_image_t *image) {
  free(image->data);
  free(image);
}

int main(int argc, char **argv) {
  if (argc < 3) {
    print_usage(argv[0]);
    return EXIT_SUCCESS;
  }

  if (strcmp(argv[1], "info") == 0 && argc == 3) {
    const char *filename_in = argv[2];

    netpbm_image_t *image = netpbm_open(filename_in);
    if (image == NULL) {
      printf("Error: Could not read input file %s\n", filename_in);
      return EXIT_SUCCESS;
    }

    char mode[] = "unknown";
    switch (image->mode) {
    case BIT:
      strcpy(mode, "bit");
      break;
    case GRAY:
      strcpy(mode, "gray");
      break;
    case RGB:
      strcpy(mode, "rgb");
      break;
    default:
      break;
    }

    printf("Filename: %s\n", filename_in);
    printf("Size: %lux%lu\n", image->w, image->h);
    printf("Mode: %s\n", mode);
    printf("Maximum value: %d\n", image->max_val);

    return EXIT_SUCCESS;
  }

  if (strcmp(argv[1], "load") == 0 && argc >= 4) {
    if (strcmp(argv[3], "save") == 0 && argc == 5) {
      const char *filename_in = argv[2];
      const char *filename_out = argv[4];

      netpbm_image_t *image = netpbm_open(filename_in);
      if (image == NULL) {
        printf("Error: Could not read input file %s\n", filename_in);
        return EXIT_SUCCESS;
      }

      if (netpbm_save(image, filename_out) == -1) {
        printf("Error: Could not save image\n");
        netpbm_destroy(image);
        return EXIT_SUCCESS;
      }

      netpbm_destroy(image);

      return EXIT_SUCCESS;
    }
  }

  if (strcmp(argv[1], "create") == 0 && argc == 6) {
    const char *filename_out = argv[2];

    char *end = NULL;
    unsigned long long value = 0;

    value = strtoull(argv[3], &end, 10);
    if (end == argv[3] || *end != '\0') {
      printf("Error: %s is not a valid size for w\n", argv[3]);
      return EXIT_SUCCESS;
    }

    if (errno == ERANGE || value > SIZE_MAX) {
      printf("Error: %s is out of range for w\n", argv[3]);
      return EXIT_SUCCESS;
    }
    size_t w = (size_t)value;

    value = strtoull(argv[4], &end, 10);
    if (end == argv[4] || *end != '\0') {
      printf("Error: %s is not a valid size for h\n", argv[4]);
      return EXIT_SUCCESS;
    }

    if (errno == ERANGE || value > SIZE_MAX) {
      printf("Error: %s is out of range for h\n", argv[3]);
      return EXIT_SUCCESS;
    }
    size_t h = (size_t)value;

    const char *mode_str = argv[5];

    netpbm_mode_t mode = UNKNOWN;
    uint16_t max_val = 255;
    if (strcmp(mode_str, "bit") == 0) {
      mode = BIT;
      max_val = 1;
    } else if (strcmp(mode_str, "gray") == 0) {
      mode = GRAY;
      max_val = 255;
    } else if (strcmp(mode_str, "rgb") == 0) {
      mode = RGB;
      max_val = 255;
    }

    netpbm_image_t *image = netpbm_create(w, h, mode, max_val);
    if (image == NULL) {
      printf("Error: Could not create image\n");
      return EXIT_SUCCESS;
    }

    if (netpbm_save(image, filename_out) == -1) {
      printf("Error: Could not save image\n");
      netpbm_destroy(image);
      return EXIT_SUCCESS;
    }

    netpbm_destroy(image);

    return EXIT_SUCCESS;
  }

  print_usage(argv[0]);
  return EXIT_SUCCESS;
}
