%{
#include "netpbm.h"

#define BLANK   0
#define MAGICAL 1
#define NUMBER  2
%}

%option noyywrap

%%

[ \t\r\n]         { return BLANK; }
P[1-6]            { return MAGICAL; }
[1-9][0-9]*       { return NUMBER; }

%%

// Tries to read a netpbm header in file f
// Returns NULL on error, or the image on success
netpbm_image_t* parse_header() {
  if (yylex() != MAGICAL)
    return NULL;

  netpbm_mode_t mode;
  if (yytext[1] == '4')
    mode = NETPBM_MODE_BIT;
  else if (yytext[1] == '5')
    mode = NETPBM_MODE_GRAY;
  else if (yytext[1] == '6')
    mode = NETPBM_MODE_RGB;
  else
    mode = NETPBM_MODE_UNKNOWN;

  char *end = NULL;
  unsigned long long value = 0;
  int token = -1;

  if (yylex() != BLANK) return NULL;
  while ((token = yylex()) == BLANK) {}

  if (token != NUMBER)
    return NULL;

  value = strtoull(yytext, &end, 10);
  if (end == yytext || *end != '\0') {
    printf("Error: %s is not a valid size for w\n", yytext);
    return NULL;
  }

  if (errno == ERANGE || value > SIZE_MAX) {
    printf("Error: %s is out of range for w\n", yytext);
    return NULL;
  }
  size_t w = (size_t)value;

  if (yylex() != BLANK) return NULL;
  while ((token = yylex()) == BLANK) {}
  
  if (token != NUMBER)
    return NULL;

  value = strtoull(yytext, &end, 10);
  if (end == yytext || *end != '\0') {
    printf("Error: %s is not a valid size for h\n", yytext);
    return NULL;
  }

  if (errno == ERANGE || value > SIZE_MAX) {
    printf("Error: %s is out of range for h\n", yytext);
    return NULL;
  }
  size_t h = (size_t)value;

  if (yylex() != BLANK) return NULL;
  while ((token = yylex()) == BLANK) {}
  
  if (token != NUMBER)
    return NULL;

  value = strtoull(yytext, &end, 10);
  if (end == yytext || *end != '\0') {
    printf("Error: %s is not a valid size for max value\n", yytext);
    return NULL;
  }

  if (errno == ERANGE || value > UINT16_MAX) {
    printf("Error: %s is out of range for max value\n", yytext);
    return NULL;
  }
  uint16_t max_val = (uint16_t)value;

  if (yylex() != BLANK) return NULL;
  
  netpbm_image_t *image = netpbm_create(w, h, mode, max_val);
  if (image == NULL) {
    printf("Error: Could not create image\n");
    return NULL;
  }
  
  return image;
}

// Tries to read netpbm image data and updates data field
// Returns -1 on error, or 0 on success
int parse_data(netpbm_image_t *image) {
  size_t w = netpbm_get_width(image);
  size_t h = netpbm_get_height(image);
  netpbm_mode_t mode = netpbm_get_mode(image);
  uint16_t max_val = netpbm_get_max_val(image);

  if (w == 0 || h == 0)
    return 0;

  size_t total = w * h * (mode == NETPBM_MODE_RGB ? 3 : 1);
  if (max_val < 256) {
    for (size_t i = 0; i < total; i++) {
      netpbm_set_raw(image, i, input());
      if (netpbm_get_raw(image, i) > max_val) {
        printf("Error: Found a data sample greater than indicated maximum value");
        return -1;
      }
    }
  } else {
    for (size_t i = 0; i < total; i++) {
      uint16_t msb = input();
      uint16_t lsb = input();
      netpbm_set_raw(image, i, (msb << 8) | lsb);
      if (netpbm_get_raw(image, i) > max_val) {
        printf("Error: Found a data sample greater than indicated maximum value");
        return -1;
      }
    }
  }

  return 0;
}

netpbm_image_t *netpbm_open(const char *filename) {
  if (filename == NULL) {
    printf("Error: Could not open input file\n");
    return NULL;
  }

  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    printf("Error: Could not open input file\n");
    return NULL;
  }

  yyin = f;

  netpbm_image_t *image = parse_header();
  if (image == NULL) {
    printf("Error: Could not parse input file header\n");
    return NULL;
  }

  if (parse_data(image) == -1) {
    printf("Error: Could not read input file image data\n");
    netpbm_destroy(image);
    return NULL;
  }

  fclose(f);
  return image;
}
