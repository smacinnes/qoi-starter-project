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


#define HEADER_SIZE    14
#define FOOTER_SIZE     8
#define BYTES_PER_PIXEL 4

// byte indexes into a pixel
#define RED   0
#define GREEN 1
#define BLUE  2
#define ALPHA 3

// lengths of chunk sizes in qoi format
#define CHUNK_LEN_RGB   4
#define CHUNK_LEN_RGBA  5
#define CHUNK_LEN_INDEX 1
#define CHUNK_LEN_DIFF  1
#define CHUNK_LEN_LUMA  2
#define CHUNK_LEN_RUN   1

// bit masks to extract info from chunks
#define INDEX_MASK  0b00111111
#define DIFF_R_MASK 0b00110000
#define DIFF_G_MASK 0b00001100
#define DIFF_B_MASK 0b00000011
#define LUMA_R_MASK 0b11110000
#define LUMA_G_MASK 0b00111111
#define LUMA_B_MASK 0b00001111
#define RUN_MASK    0b00111111

// shift sizes after masks
#define DIFF_R_SHIFT 4
#define DIFF_G_SHIFT 2
#define DIFF_B_SHIFT 0
#define LUMA_R_SHIFT 4

// biases used to encode signed offsets as unsigned bytes
#define DIFF_BIAS    2
#define LUMA_G_BIAS 32
#define LUMA_RB_BIAS 8
#define RUN_BIAS    -1

// hash function parameters
#define HASH_SIZE 64
#define HASH_R 3
#define HASH_G 5
#define HASH_B 7
#define HASH_A 11

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
void *qoi_decode(void const *data, uint64_t size, qoi_desc_t *out_desc);
