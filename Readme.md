# netpbm

A lightweight C library for reading, writing, and manipulating **Netpbm image formats** - PBM (bitmap), PGM (grayscale), and PPM (RGB color).

## Table of Contents

- [About](#about)
- [Features](#features)
- [Installation](#installation)
- [Usage Overview](#usage-overview)
- [Public API](#public-api)
    - [Image Creation and Destruction](#image-creation-and-destruction)
    - [File I/O](#file-io)
    - [Mode Conversion](#mode-conversion)
- [Examples](#examples)
- [Future Directions](#future-directions)
- [Contributing](#contributing)
- [License](#license)

## About

**netpbm** provides a small C interface to manipulate images in the [Netpbm family of formats](https://en.wikipedia.org/w/index.php?title=Netpbm):

- **PBM** - black & white bitmap
- **PGM** - grayscale image
- **PPM** - RGB color image

The library focuses on the raw binary encoding. It is meant to be a helper library for other projects based on the netpbm formats.

## Features

- Simple, minimal C API
- Read/write PBM, PGM, and PPM images
- Convert between black and white, grayscale and RGB modes
- Memory-safe allocation and cleanup functions

## Installation

Clone and build using `make`:

```bash
    git clone https://github.com/champiop/netpbm.git
    cd netpbm
    make
```

Include the header in your project:

```c
    #include "netpbm.h"
```

Compile and link with your project:

```bash
    gcc -o example example.c -I/path/to/netpbm -L/path/to/netpbm -lnetpbm
```

## Usage Overview

A typical workflow:

1. **Create** or **load** a Netpbm image (`netpbm_create`/`netpbm_open`)
2. **Manipulate** or **convert** the image (`netpbm_togray`/`netpbm_torgb`)
3. **Save** the result (`netpbm_save`)
4. **Free** resources (`netpbm_destroy`)

## Public API

### Image Creation and Destruction

`netpbm_image_t *netpbm_create(size_t w, size_t h, netpbm_mode_t mode, uint16_t max_val);`

Creates a new image with the given width, height, color mode, and maximum sample value.

Returns:

- Pointer to a new image on success
- `NULL` on allocation error

---

`void netpbm_destroy(netpbm_image_t *image);`

Frees memory and resources associated with an image.
Not safe to call with `NULL`.

### File I/O

`netpbm_image_t *netpbm_open(const char *filename);`

Opens a Netpbm file (`.pbm`, `.pgm`, `.ppm`) and returns a new image object.

Returns:

- Pointer to the image object on success
- `NULL` on read or format error

---

`int netpbm_save(netpbm_image_t *image, const char *filename);`

Saves the given image to the specified file in the correct Netpbm format.

Returns:

- `0` on success
- `-1` on I/O or format error

### Mode Conversion

`int netpbm_togray(netpbm_image_t *image);`

Converts the image to grayscale (PGM).

- RGB images are converted averaging the color samples.
- Bitmaps are promoted to grayscale with maximum sample value of 255.

Returns:

- `0` on success
- `-1` on error (unknown initial color mode)

---

`int netpbm_torgb(netpbm_image_t *image);`

Converts the image to RGB color (PPM).

- Grayscale images are expanded to three channels.
- Bitmaps are promoted to rgb with maximum sample value of 255.

Returns:

- `0` on success
- `-1` on error (buffer size overflow or unknown initial color mode)

## Examples

```c
#include "netpbm.h"
#include <stdio.h>

int main(void) {
    // Create a 100x100 grayscale image with 8-bit samples
    netpbm_image_t *img = netpbm_create(100, 100, GRAY, 255);
    if (!img) {
        fprintf(stderr, "Failed to create image\n");
        return 1;
    }

    // (Modify pixels here...)

    // Save to file
    if (netpbm_save(img, "output.pgm") != 0) {
        fprintf(stderr, "Failed to save image\n");
    }

    netpbm_destroy(img);
    return 0;
}
```

To load and convert:

```c
netpbm_image_t *img = netpbm_open("input.ppm");
if (img) {
    netpbm_togray(img);
    netpbm_save(img, "gray.pgm");
    netpbm_destroy(img);
}
```

## Future Directions

- Support for PAM and alpha channel
- Optional plain ASCII format support

## Contributing

Contributions are welcome!
- Add more format variants
- Extend conversion functions
- Add test coverage

Submit a pull request or open an issue.

## License

This project is licensed under the [MIT License](LICENSE).

You are free to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of this software, provided that you include the original copyright notice
and this permission notice in all copies or substantial portions of the software.

See the [LICENSE](LICENSE) file for full details.
