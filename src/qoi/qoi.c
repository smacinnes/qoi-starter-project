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

qoi_desc_t* read_header(uint8_t const *data){
  qoi_desc_t *header = malloc(sizeof(qoi_desc_t));        // remember to clean up
  header->width = bytes_to_uint32(data+4);
  header->height = bytes_to_uint32(data+8);
  header->channels = data[12];
  header->colorspace = data[13];
  return header;
}

void print_header(qoi_desc_t *header) {
  printf("QOI File Header\n"
    "Width     : %i\n"
    "Height    : %i\n"
    "Channels  : %i\n"
    "Colorspace: %i\n", header->width, header->height, header->channels, header->colorspace);
}

enum opcode decode_chunk(uint8_t const *chunk) {

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
    // printf("RGB");
    return QOI_OP_RGB;
  case 0xFF:
    // printf("RGBA");
    return QOI_OP_RGBA;
  default:
  {
    uint8_t opcode = first_byte >> 6;
    switch (opcode) {
    case 0x00:
      // printf("Index");
      return QOI_OP_INDEX;
    case 0x01:
      // printf("Diff");
      return QOI_OP_DIFF;
    case 0x02:
      // printf("Luma");
      return QOI_OP_LUMA;
    case 0x03:
      // printf("Run");
      return QOI_OP_RUN;
    default:
      printf("ERROR: Opcode not recognized");
      break;
    }
  }
    break;
  }
  printf("\n");
}


uint8_t *qoi_decode(uint8_t const *data, uint64_t size, qoi_desc_t *out_desc) {
  
  out_desc = read_header(data);
  
  char magic[5];
  strncpy(magic, data, 4);
  magic[4] = '\0';                                // null terminate for strcmp
  if (strcmp(magic, "qoif") != 0) {
    printf("Invalid file type\n");
    return NULL;
  }
  print_header(out_desc);


  uint8_t channels = out_desc->channels;

  uint8_t prev_pixels[64 * 4] = {0};  // 64 previous pixels with hash map

  uint8_t *decoded_data = calloc((size_t)(out_desc->width * out_desc->height * 4), 1);
  decoded_data[3] = 255; // set initial alpha to 255? will this work?

  uint8_t *next_read = &data[HEADER_SIZE];  // header in dice.qoi is followed by 0x00?

  uint8_t *next_write = decoded_data;   // points to start of next pixel
  uint8_t *prev_write = decoded_data;   // points to start of prev pixel


  int chunks = 0;
  printf("%i\n", next_read);
  printf("%i\n", data);
  printf("%i\n", size);
  while (next_read < (data+size)){
    


    // printf("%i\n", *next_byte);
    enum opcode op = decode_chunk(next_read);

    switch (op)
    {
    case QOI_OP_RGB:
      memcpy(next_write, next_read+1, 3);   // &x[1] vs x+1
      next_write[3] = prev_write[3];  // copy prev alpha
      next_read += 3;
      break;
    case QOI_OP_RGBA:
      memcpy(next_write, next_read+1, 4);
      next_read += 4;
      break;
    case QOI_OP_INDEX: {

      uint8_t index = (*next_read) & 0b00111111;
      // next_read = &prev_pixels[index * 4];
      memcpy(next_write, &prev_pixels[index * 4]+1, 3);
      next_write[3] = prev_write[3];  // copy prev alpha
      next_read += 1;
      break;
    }
    case QOI_OP_DIFF: {

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
      next_read += 1;
      break;
    }
    case QOI_OP_LUMA:
    {

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
      next_read += 2;
      break;
    }
    case QOI_OP_RUN:
    {

      uint8_t length = (*next_read) & 0b00111111 + 1;
      for (uint8_t i=0;i<length-1;i++) {  // better way?
        memcpy(next_write,prev_write,4);
        next_write += 4;
      }
      memcpy(next_write,prev_write,4); // so don't break the next pointer advance
      next_read += 1;
      break;
    }
    }

    // add last pixel to hash map
    uint8_t index_position = (*(prev_write) * 3 + *(prev_write+1) * 5 + *(prev_write+2) * 7 + *(prev_write+3) * 11) % 64;
    memcpy(&prev_pixels[index_position], next_write, 4);


    // advance pointers
    prev_write = next_write;  // still okay if was a RUN as all written pixels identical 
    next_write += 4;

    chunks++;

  }
  printf("%i\n", chunks);
  return decoded_data;
}
