#include "qoi/qoi.h"

#include "qoi/details/unused.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// bit shift two bytes into one 16-bit number (BE)
uint16_t bytes_to_uint16(uint8_t const *data) {
  return (uint16_t)data[1] | (uint16_t)data[0] << 8;
}

// bit shift four bytes into one 32-bit number (BE)
uint32_t bytes_to_uint32(uint8_t const *data) {
  return (uint32_t)data[3] | (uint32_t)data[2] << 8 
    | (uint32_t)data[1] << 16 | (uint32_t)data[0] << 24;
}

// read the file header into the description struct
void read_header(qoi_desc_t *out_desc, uint8_t const *data){
  out_desc->width = bytes_to_uint32(data+4);
  out_desc->height = bytes_to_uint32(data+8);
  out_desc->channels = data[12];
  out_desc->colorspace = data[13];
}

// print the file header
void print_header(qoi_desc_t *header) {
  printf("QOI File Header\n"
    "Width     : %i\n"
    "Height    : %i\n"
    "Channels  : %i\n"
    "Colorspace: %i\n", header->width, header->height, header->channels, header->colorspace);
}

// check the first four bytes match "qoif"
int is_valid_file_type(uint8_t const *data) {
  char magic[5];
  strncpy(magic, data, 4);
  magic[4] = '\0';          // null terminate for strcmp
  return (strcmp(magic, "qoif") == 0);
}

// identify the type of chunk by examining the first byte
enum opcode get_opcode(uint8_t const *chunk) {

  uint8_t first_byte = chunk[0];
  switch (first_byte) {  // check for 8-bit opcodes first
  case 0xFE:
    return QOI_OP_RGB;
  case 0xFF:
    return QOI_OP_RGBA;
  default:
  {
    uint8_t opcode = first_byte >> 6;  // check for 2-bit opcodes
    switch (opcode) {
    case 0x00:
      return QOI_OP_INDEX;
    case 0x01:
      return QOI_OP_DIFF;
    case 0x02:
      return QOI_OP_LUMA;
    case 0x03:
      return QOI_OP_RUN;
    default:
      printf("ERROR: Opcode not recognized: %x", first_byte);
      exit(1);
    }
  }
  }
}

// decode the RGB chunk
void decode_RGB(uint8_t const *next_read, uint8_t *next_write, uint8_t *prev_write) {
  memcpy(next_write, next_read+1, 3);  // skip opcode byte and copy next three (RGB values)
  next_write[ALPHA] = prev_write[ALPHA];
}

// decode the RGBA chunk
void decode_RGBA(uint8_t const *next_read, uint8_t *next_write) {
  memcpy(next_write, next_read+1, BYTES_PER_PIXEL);  // skip opcode byte and copy next four (RGBA values)
}

// decode the INDEX chunk
void decode_INDEX(uint8_t const *next_read, uint8_t *next_write, uint8_t *prev_pixels) {
  uint8_t index = (*next_read) & INDEX_MASK;
  memcpy(next_write, &prev_pixels[index*BYTES_PER_PIXEL], BYTES_PER_PIXEL);
}

// decode the DIFF chunk
void decode_DIFF(uint8_t const *next_read, uint8_t *next_write, uint8_t *prev_write) {
  int8_t diff_red   = (((*next_read) & DIFF_R_MASK) >> DIFF_R_SHIFT) - DIFF_BIAS;
  int8_t diff_green = (((*next_read) & DIFF_G_MASK) >> DIFF_G_SHIFT) - DIFF_BIAS;
  int8_t diff_blue  = (((*next_read) & DIFF_B_MASK) >> DIFF_B_SHIFT) - DIFF_BIAS;
  next_write[RED]   = prev_write[RED]   + diff_red;
  next_write[GREEN] = prev_write[GREEN] + diff_green;
  next_write[BLUE]  = prev_write[BLUE]  + diff_blue;
  next_write[ALPHA] = prev_write[ALPHA];
}

// decode the LUMA chunk
void decode_LUMA(uint8_t const *next_read, uint8_t *next_write, uint8_t *prev_write) {
  int8_t diff_green = ((*next_read)    & LUMA_G_MASK) - LUMA_G_BIAS;
  int8_t diff_red   = ((*(next_read+1) & LUMA_R_MASK) >> LUMA_R_SHIFT) + diff_green - LUMA_RB_BIAS;
  int8_t diff_blue  = ( *(next_read+1) & LUMA_B_MASK) + diff_green - LUMA_RB_BIAS;
  next_write[RED]   = prev_write[RED]   + diff_red;
  next_write[GREEN] = prev_write[GREEN] + diff_green;
  next_write[BLUE]  = prev_write[BLUE]  + diff_blue;
  next_write[ALPHA] = prev_write[ALPHA];
}

// decode the RUN chunk
uint8_t decode_RUN(uint8_t const *next_read, uint8_t *next_write, uint8_t *prev_write) {
  uint8_t length = ((*next_read) & RUN_MASK) - RUN_BIAS;
  for (uint8_t i=0;i<length;i++) {
    memcpy(next_write,prev_write,BYTES_PER_PIXEL); 
    next_write += BYTES_PER_PIXEL;
  }
  return length;
}

// add pixel to hash map
void hash_pixel(uint8_t *pixel, uint8_t *prev_pixels) {
  uint8_t index = (pixel[RED] * HASH_R + pixel[GREEN] * HASH_G + pixel[BLUE] * HASH_B + pixel[ALPHA] * HASH_A) % HASH_SIZE;
  memcpy(&prev_pixels[index*BYTES_PER_PIXEL], pixel, BYTES_PER_PIXEL);
}

void *qoi_decode(void const *data, uint64_t size, qoi_desc_t *out_desc) {

  /*
  TODO:
  build and run debug with one key press
  fix warnings about const
  basic error warnings?
  formatting
  multiple files?
  */

  if (!is_valid_file_type(data)) {
    printf("Invalid file type\n");
    return NULL;
  }

  read_header(out_desc, data);
  print_header(out_desc);

  uint8_t prev_pixels[HASH_SIZE * BYTES_PER_PIXEL] = {0};  // 64 previous pixels with hash map

  uint8_t *decoded_data = calloc((size_t)(out_desc->width * out_desc->height), BYTES_PER_PIXEL);
  decoded_data[ALPHA] = 255; // set initial alpha to fully opaque

  uint8_t const *next_read = &data[HEADER_SIZE];  // next_read is const?

  uint8_t *next_write = decoded_data;   // points to start of next pixel
  uint8_t *prev_write = decoded_data;   // points to start of prev pixel

  unsigned chunks = 0;
  unsigned pixels = 0;
  
  printf("Encoded data: %u bytes\n", size);
  
  while (next_read < (data+size-FOOTER_SIZE)){  // todo: detect end condition

    enum opcode op = get_opcode(next_read);

    switch (op) {
    case QOI_OP_RGB:
      decode_RGB(next_read, next_write, prev_write);
      next_read += CHUNK_LEN_RGB;
      break;
    case QOI_OP_RGBA:
      decode_RGBA(next_read, next_write);
      next_read += CHUNK_LEN_RGBA;
      break;
    case QOI_OP_INDEX:
      decode_INDEX(next_read, next_write, prev_pixels);
      next_read += CHUNK_LEN_INDEX;
      break;
    case QOI_OP_DIFF:
      decode_DIFF(next_read, next_write, prev_write);
      next_read += CHUNK_LEN_DIFF;
      break;
    case QOI_OP_LUMA:
      decode_LUMA(next_read, next_write, prev_write);
      next_read += CHUNK_LEN_LUMA;
      break;
    case QOI_OP_RUN: {
      uint8_t length = decode_RUN(next_read, next_write, prev_write);
      next_read += CHUNK_LEN_RUN;
      next_write += (length-1) * BYTES_PER_PIXEL;  // last increment happens later 
      pixels += (length-1);
      break;
    }
    }

    // adds pixel to hash map
    hash_pixel(next_write, prev_pixels);

    // advance pointers
    prev_write = next_write;
    next_write += BYTES_PER_PIXEL;

    // track progress
    chunks++;
    pixels++;
  }
  printf("Decoded data:\n"
         "Bytes : %u\n"
         "Chunks: %u\n"
         "Pixels: %u\n", (out_desc->width * out_desc->height * 4), chunks, pixels);
  return decoded_data;
}
