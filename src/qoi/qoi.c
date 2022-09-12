#include "qoi/qoi.h"

#include "qoi/details/unused.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint16_t bytes_to_uint16(uint8_t const *data) {
  return (uint16_t)data[1] | (uint16_t)data[0] << 8;
}

uint32_t bytes_to_uint32(uint8_t const *data) {
  return (uint32_t)data[3] | (uint32_t)data[2] << 8 
    | (uint32_t)data[1] << 16 | (uint32_t)data[0] << 24;
}

void read_header(qoi_desc_t *out_desc, uint8_t const *data){
  out_desc->width = bytes_to_uint32(data+4);
  out_desc->height = bytes_to_uint32(data+8);
  out_desc->channels = data[12];
  out_desc->colorspace = data[13];
}

void print_header(qoi_desc_t *header) {
  printf("QOI File Header\n"
    "Width     : %i\n"
    "Height    : %i\n"
    "Channels  : %i\n"
    "Colorspace: %i\n", header->width, header->height, header->channels, header->colorspace);
}

int is_valid_file_type(uint8_t const *data) {
  char magic[5];
  strncpy(magic, data, 4);
  magic[4] = '\0';                                // null terminate for strcmp
  return (strcmp(magic, "qoif") == 0);
  
  // if (strcmp(magic, "qoif") != 0) {
  //   printf("Invalid file type\n");
  //   return -1;
  // }
  // return 0;
}

enum opcode get_opcode(uint8_t const *chunk) {

  uint8_t first_byte = chunk[0];
  // uint8_t first_byte = 0b11111110;  // rgb
  // uint8_t first_byte = 0b11111111;  // rgba
  // uint8_t first_byte = 0b00111111;  // index
  // uint8_t first_byte = 0b01111111;  // diff
  // uint8_t first_byte = 0b10111111;  // luma
  // uint8_t first_byte = 0b11011111;  // run
  // printf("%i\n", first_byte);

  switch (first_byte) {
  case 0xFE:
    return QOI_OP_RGB;
  case 0xFF:
    return QOI_OP_RGBA;
  default:
  {
    uint8_t opcode = first_byte >> 6;
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
      printf("ERROR: Opcode not recognized");
      break;
    }
  }
    break;
  }
  // add unrecognized opcode to enum?
}

void decode_RGB(uint8_t *next_read, uint8_t *next_write, uint8_t *prev_write) {
  memcpy(next_write, next_read+1, 3);   // &x[1] vs x+1
  next_write[3] = prev_write[3];  // copy prev alpha
}

void decode_RGBA(uint8_t *next_read, uint8_t *next_write, uint8_t *prev_write) {
  memcpy(next_write, next_read+1, 4);
}

void decode_INDEX(uint8_t *next_read, uint8_t *next_write, uint8_t *prev_write, uint8_t *prev_pixels) {
  uint8_t index = (*next_read) & 0b00111111;
  memcpy(next_write, &prev_pixels[index * 4]+1, 3);
  next_write[3] = prev_write[3];  // copy prev alpha
}

void decode_DIFF(uint8_t *next_read, uint8_t *next_write, uint8_t *prev_write) {
  // static?
  int8_t diff_red   = ((*next_read) & 0b00110000) - 2;
  int8_t diff_green = ((*next_read) & 0b00001100) - 2;
  int8_t diff_blue  = ((*next_read) & 0b00000011) - 2;
  uint8_t red   = *(prev_write+0) + diff_red;
  uint8_t green = *(prev_write+1) + diff_green;
  uint8_t blue  = *(prev_write+2) + diff_blue;
  next_write[0] = red;
  next_write[1] = green;
  next_write[2] = blue;
  next_write[3] = prev_write[3];  // copy prev alpha
}

void decode_LUMA(uint8_t *next_read, uint8_t *next_write, uint8_t *prev_write) {
  int8_t diff_green = ((*next_read) & 0b00111111) - 32;
  int8_t diff_red   = ((*next_read+1) & 0b11110000) + diff_green - 8;
  int8_t diff_blue  = ((*next_read+1) & 0b00001111) + diff_green - 8;
  uint8_t red   = *(prev_write+0) + diff_red;
  uint8_t green = *(prev_write+1) + diff_green;
  uint8_t blue  = *(prev_write+2) + diff_blue;
  next_write[0] = red;
  next_write[1] = green;
  next_write[2] = blue;
  next_write[3] = prev_write[3];  // copy prev alpha
}

uint8_t decode_RUN(uint8_t *next_read, uint8_t *next_write, uint8_t *prev_write) {
  uint8_t length = ((*next_read) & 0b00111111) + 1;
  for (uint8_t i=0;i<length;i++) {  // better way?
    memcpy(next_write,prev_write,4); 
    // fails at i=41, len=57, 0x91b532, chunks=46943
    next_write += 4;  // pass by ref or return how many were advanced?
  }
  return length;
}


uint8_t *qoi_decode(uint8_t const *data, uint64_t size, qoi_desc_t *out_desc) {

  /*
  TODO:
  build and run debug with one key press
  fix warnings about const
  add more defines for biases and indexes
  basic error warnings?
  */

  if (!is_valid_file_type(data)) {
    printf("Invalid file type\n");
    return NULL;
  }

  read_header(out_desc, data);
  print_header(out_desc);

  uint8_t prev_pixels[64 * 4] = {0};  // 64 previous pixels with hash map

  uint8_t *decoded_data = calloc((size_t)(out_desc->width * out_desc->height * 4), 1);
  decoded_data[3] = 255; // set initial alpha to 255? will this work?

  uint8_t *next_read = &data[HEADER_SIZE];  // header in dice.qoi is followed by 0x00?

  uint8_t *next_write = decoded_data;   // points to start of next pixel
  uint8_t *prev_write = decoded_data;   // points to start of prev pixel


  unsigned chunks = 0;
  unsigned pixels = 0;
  
  printf("data: %u\n", data);
  printf("size: %u\n", size);
  while (next_read < (data+size-8)){
  // for (int i=0;i<5;i++){
    
    // printf("-----------\n");
    // printf("chunk     : %u\n", chunks);
    // printf("next_read : %u\n", next_read);
    // printf("next_write: %u\n", next_write);
    // printf("prev_write: %u\n", prev_write);


    // printf("%i\n", *next_byte);
    enum opcode op = get_opcode(next_read);

    // printf("opcode: %i\n", op);

    switch (op) {
    case QOI_OP_RGB:
      decode_RGB(next_read, next_write, prev_write);
      next_read += 4;
      pixels++;
      break;
    case QOI_OP_RGBA:
      decode_RGBA(next_read, next_write, prev_write);
      next_read += 5;
      pixels++;
      break;
    case QOI_OP_INDEX:
      decode_INDEX(next_read, next_write, prev_write, prev_pixels);
      next_read += 1;
      pixels++;
      break;
    case QOI_OP_DIFF:
      decode_DIFF(next_read, next_write, prev_write);
      next_read += 1;
      pixels++;
      break;
    case QOI_OP_LUMA:
      decode_LUMA(next_read, next_write, prev_write);
      next_read += 2;
      pixels++;
      break;
    case QOI_OP_RUN: {
      uint8_t length = decode_RUN(next_read, next_write, prev_write);
      next_write += (length-1) * 4;  // last increment is later 
      next_read += 1;
      pixels+=length;
      break;
    }
    }

    // add last pixel to hash map
    uint8_t index_position = (*(next_write) * 3 + *(next_write+1) * 5 + *(next_write+2) * 7 + *(next_write+3) * 11) % 64;
    memcpy(&prev_pixels[index_position*4], next_write, 4);


    // advance pointers
    prev_write = next_write;
    next_write += 4;

    chunks++;

  }
  printf("Decoded %u chunks\n", chunks);
  printf("Decoded %u pixels\n", pixels);
  return decoded_data;
}
