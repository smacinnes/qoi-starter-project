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

qoi_header* read_header(uint8_t const *data){
  qoi_header *header = malloc(sizeof(qoi_header));        // remember to clean up
  strncpy(header->magic, data, sizeof(header->magic));
  header->magic[4] = '\0';                                // null terminate for strcmp
  header->width = bytes_to_uint32(data+4);
  header->height = bytes_to_uint32(data+8);
  header->channels = data[12];
  header->colorspace = data[13];
  return header;
}

void print_header(qoi_header *header) {
  printf("QOI File Header\n"
    "Magic     : %s\n"
    "Width     : %i\n"
    "Height    : %i\n"
    "Channels  : %i\n"
    "Colorspace: %i\n", header->magic, header->width, 
    header->height, header->channels, header->colorspace);
}

void *qoi_decode(uint8_t const *data, uint64_t UNUSED(size), qoi_desc_t *UNUSED(out_desc)) {
  
  qoi_header *header = read_header(data);
    if (strcmp(header->magic, "qoif") != 0) {
    printf("Invalid file type\n");
    return NULL;
  }
  print_header(header);

  uint8_t *decoded_data = malloc((size_t)(header->width * header->height * header->channels));

  uint8_t next_byte = data[HEADER_SIZE+1];  // header in dice.qoi is followed by 0x00?

  printf("%i\n", next_byte);

  
  return NULL;
}
