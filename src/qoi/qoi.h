#pragma once

#include <stdint.h>

/// \file qoi/qoi.h
///
/// Public interface of QOI codec library

/// @defgroup colorspace Colorspaces
/// QOI supports two colorspaces: sRGB and linear
/// @{
#define QOI_COLORSPACE_SRGB 0    ///< sRGB with linear alpha
#define QOI_COLORSPACE_LINEAR 1  ///< all channels linear
/// @}

#define HEADER_SIZE 14


/// Image description
typedef struct qoi_desc_t {
  uint32_t width;      ///< image width, in pixels
  uint32_t height;     ///< image height, in pixels
  uint8_t channels;    ///< number of channels: 3 (RGB) or 4 (RGBA)
  uint8_t colorspace;  ///< image colorspace: see @ref colorspace
} qoi_desc_t;

/// File header
typedef struct qoi_header {
  char magic[4+1];      // magic bytes "qoif" with added null termination
  uint32_t width;       // image width in pixels (BE)
  uint32_t height;      // image height in pixels (BE)
  uint8_t channels;     // 3 = RGB, 4 = RGBA
  uint8_t colorspace;   // 0 = sRGB with linear alpha
                        // 1 = all channels linear
} qoi_header;

/// Decodes the provided buffer into raw pixels
/// \returns A pointer to the decoded image. The caller is responsible for freeing this memory.
/// \param data A pointer to the buffer of pixels to decode
/// \param size The size of the input buffer, in bytes
/// \param[out] out_desc The description of the decoded image
void *qoi_decode(uint8_t const *data, uint64_t size, qoi_desc_t *out_desc);
