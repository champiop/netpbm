#include "netpbm.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

// Displays netpbm utility man
void print_usage(const char *exe_name) {
  printf("netpbm utility\n"
         "Usage:\n"
         "  %s info   <filename_in>\n"
         "  %s load   <filename_in> save <filename_out>\n"
         "  %s togray <filename_in> save <filename_out>\n"
         "  %s torgb  <filename_in> save <filename_out>\n"
         "  %s create <filename_out> <w> <h> <mode>\n"
         "            (mode is 'bit', 'gray' or 'rgb')\n",
         exe_name, exe_name, exe_name, exe_name, exe_name);
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
    switch (netpbm_get_mode(image)) {
    case NETPBM_MODE_BIT:
      strcpy(mode, "bit");
      break;
    case NETPBM_MODE_GRAY:
      strcpy(mode, "gray");
      break;
    case NETPBM_MODE_RGB:
      strcpy(mode, "rgb");
      break;
    default:
      break;
    }

    printf("Filename: %s\n", filename_in);
    printf("Size: %lux%lu\n", netpbm_get_width(image),
           netpbm_get_height(image));
    printf("Mode: %s\n", mode);
    printf("Maximum value: %d\n", netpbm_get_max_val(image));

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

  if (strcmp(argv[1], "togray") == 0 && argc >= 4) {
    if (strcmp(argv[3], "save") == 0 && argc == 5) {
      const char *filename_in = argv[2];
      const char *filename_out = argv[4];

      netpbm_image_t *image = netpbm_open(filename_in);
      if (image == NULL) {
        printf("Error: Could not read input file %s\n", filename_in);
        return EXIT_SUCCESS;
      }

      if (netpbm_togray(image) == -1) {
        printf("Error: Could not convert image to gray mode\n");
        netpbm_destroy(image);
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

  if (strcmp(argv[1], "torgb") == 0 && argc >= 4) {
    if (strcmp(argv[3], "save") == 0 && argc == 5) {
      const char *filename_in = argv[2];
      const char *filename_out = argv[4];

      netpbm_image_t *image = netpbm_open(filename_in);
      if (image == NULL) {
        printf("Error: Could not read input file %s\n", filename_in);
        return EXIT_SUCCESS;
      }

      if (netpbm_torgb(image) == -1) {
        printf("Error: Could not convert image to rgb mode\n");
        netpbm_destroy(image);
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

    netpbm_mode_t mode = NETPBM_MODE_UNKNOWN;
    uint16_t max_val = 255;
    if (strcmp(mode_str, "bit") == 0) {
      mode = NETPBM_MODE_BIT;
      max_val = 1;
    } else if (strcmp(mode_str, "gray") == 0) {
      mode = NETPBM_MODE_GRAY;
      max_val = 255;
    } else if (strcmp(mode_str, "rgb") == 0) {
      mode = NETPBM_MODE_RGB;
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
