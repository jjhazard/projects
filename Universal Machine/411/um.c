#include <stdio.h>
#include <stdlib.h>
#include "umDelegator.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "Incorrect argument number.\n");
    exit(1);
  }

  executeUM(argv[1]);
}
