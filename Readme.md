# netpbm

A lightweight C library for reading, writing, and manipulating **Netpbm image formats** - PBM (bitmap), PGM (grayscale), and PPM (RGB color).

## Table of Contents

- [About]()
- [Features]()
- [Installation]()
- [Usage Overview]()
- [Public API]
    - [Image Creation and Destruction]()
    - [File I/O]()
    - [Mode Conversion]()
- [Examples]()
- [Future Directions]()
- [Contributing]()
- [License]()

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

`
    git clone https://github.com/champiop/netpbm.git
    cd netpbm
    make
`

Include the header in your project:

`
    #include "netpbm.h"
`

Compile and link with your project:

`
    gcc -o example example.c -I/path/to/netpbm -L/path/to/netpbm -lnetpbm
`

## Usage Overview

A typical workflow:

1. **Create** or **load** a Netpbm image (`netpbm_create`/`netpbm_open`)
2. **Manipulate** or **convert** the image (`netpbm_togray`/`netpbm_torgb`)
3. **Save** the result (`netpbm_save`)
4. **Free** resources (`netpbm_destroy`)

## Public API

> ⚠️Note: The internal layout of `netpbm_image_t` and `netpbm_mode_t` should be treated as **opaque**. Only manipulate the data structure through defined functions. Future versions may hide these definitions behind accessors.

### Image Creation and Destruction

`netpbm_image_t *netpbm_create(size_t w, size_t h, netpbm_mode_t mode, uint16_t max_val);`

Creates a new image with the given width, height, color mode, and maximum sample value.

Returns:

- Pointer to a new image on success
- `NULL` on allocation error

