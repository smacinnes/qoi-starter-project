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

#define CHUNK_LEN_RGB 4
#define CHUNK_LEN_RGBA 5
#define CHUNK_LEN_INDEX 1
#define CHUNK_LEN_DIFF 1
#define CHUNK_LEN_LUMA 2
#define CHUNK_LEN_RUN 1

/// Image description
typedef struct qoi_desc_t {
  uint32_t width;      ///< image width, in pixels
  uint32_t height;     ///< image height, in pixels
  uint8_t channels;    ///< number of channels: 3 (RGB) or 4 (RGBA)
  uint8_t colorspace;  ///< image colorspace: see @ref colorspace
} qoi_desc_t;

enum opcode{
  QOI_OP_RGB,
  QOI_OP_RGBA,
  QOI_OP_INDEX,
  QOI_OP_DIFF,
  QOI_OP_LUMA,
  QOI_OP_RUN
};


/// Decodes the provided buffer into raw pixels
/// \returns A pointer to the decoded image. The caller is responsible for freeing this memory.
/// \param data A pointer to the buffer of pixels to decode
/// \param size The size of the input buffer, in bytes
/// \param[out] out_desc The description of the decoded image
uint8_t *qoi_decode(uint8_t const *data, uint64_t size, qoi_desc_t *out_desc);
