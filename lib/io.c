#include "io.h"
#include <stdio.h>
#include <unistd.h>

char io_read_char(void) {
  char r = 0;
  read(STDIN_FILENO, &r, 1);
  if (r == 0) {
    io_print_err(
        "failed to read character of size 1 from stdin - returning -1");
    return -1;
  }
  return r;
}

void io_print_err(char *s) { fprintf(stderr, "err: %s\n", s); }
