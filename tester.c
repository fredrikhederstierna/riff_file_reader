/**
 * Simple test program for RIFF file reader
 *
 * Fredrik Hederstierna 2021
 */

#include <stdio.h>

#include <riff_file_reader.h>

//--------------------------------------------------
static void indent(int32_t level)
{
  int i;
  for (i = 0; i < level; i++) {
    printf("||");
  }
}

static void riff_file_list_chunk_start_fn(riff_file_data_chunk_iterator_h iter_h,
                                          int level,
                                          const char type[4],
                                          size_t size,
                                          const char format[4])
{
  indent(level); printf(" p--LIST.START[%d]: TYPE <%c%c%c%c> SIZE(%d) FORMAT <%c%c%c%c>\n",
                        level,
                        type[0], type[1], type[2], type[3],
                        (int)size,
                        format[0], format[1], format[2], format[3]);
}
static void riff_file_list_chunk_end_fn(riff_file_data_chunk_iterator_h iter_h,
                                        int level)
{
  indent(level); printf(" b--LIST.END[%d].\n", level);
}

//--------------------------------------------------
int main (int argc, char **argv)
{
  printf("RIFF file reader test\n");

  if (argc < 3) {
    printf("Usage: %s filename type\n", argv[0]);
    return 0;
  }

  char *filename = argv[1];
  char type[4];
  type[0] = argv[2][0];
  type[1] = argv[2][1];
  type[2] = argv[2][2];
  type[3] = argv[2][3];

  printf("Filename %s filename type %c%c%c%c\n",
         filename, type[0], type[1], type[2], type[3]);
  
  riff_file_h rf = riff_file_open(filename, type);
  if (rf != NULL) {
    riff_file_data_chunk_iterator_h iter_h = riff_file_data_chunk_iterator_new(rf,
                                                                               riff_file_list_chunk_start_fn,
                                                                               riff_file_list_chunk_end_fn);
    if (iter_h != NULL) {
      struct riff_file_data_subchunk_s* chunk;
      printf("---------------------------------------\n");
      do {
        chunk = riff_file_data_chunk_iterator_next(iter_h);
        if (chunk != NULL) {
          int32_t level = riff_file_data_chunk_iterator_get_list_level(iter_h);
          indent(level+1); printf("....CHUNK: ID <%c%c%c%c> SIZE(%d) OBJ(0x%016lx)\n",
                                  chunk->id[0], chunk->id[1], chunk->id[2], chunk->id[3],
                                  chunk->size,
                                  (intptr_t)chunk);
          // dump data
          uint32_t i;
          uint32_t len = chunk->size;
          if (len > 16) {
            len = 16;
          }
          indent(level+1); printf("....DATA : [");
          for (i = 0; i < len; i++) {
            if (i > 0) {
              printf(" ");
            }
            printf("%02x", chunk->data[i]);
          }
          if (chunk->size > len) {
            printf("...");
          }
          printf("]\n");
        }
        else {
          printf("EOF.\n");
          printf("---------------------------------------\n");
        }
      } while (chunk != NULL);

      int32_t res = riff_file_data_chunk_iterator_delete(iter_h);
      if (res != 0) {
        perror("iterator delete fail");
      }
      res = riff_file_close(rf);
      if (res != 0) {
        perror("file close fail");
      }
    }
  }

  return 0;
}
