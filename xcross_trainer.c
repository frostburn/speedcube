#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"

#include "cube.c"
#include "moves.c"
#include "sequence.c"
#include "locdir.c"
#include "tablebase.c"

int main() {
  srand(time(NULL));

  FILE *fptr;
  size_t num_read;
  size_t tablebase_size;

  fprintf(stderr, "Loading tablebase for xcross...\n");
  Nibblebase tablebase = init_nibblebase(LOCDIR_XCROSS_INDEX_SPACE, &locdir_xcross_index);
  #ifdef SCISSORS_ENABLED
  fptr = fopen("./tables/xcross_scissors.bin", "rb");
  #else
  fptr = fopen("./tables/xcross.bin", "rb");
  #endif
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_XCROSS_INDEX_SPACE + 1)/2;
  num_read = fread(tablebase.octets, sizeof(unsigned char), tablebase_size, fptr);
  if (num_read != tablebase_size) {
    fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, tablebase_size);
    exit(EXIT_FAILURE);
  }
  fclose(fptr);

  LocDirCube ldc;
  Cube cube;

  printf("Hold the cube with the green face towards you and the white face pointing down and apply the given sequence.\n");
  printf("The objective is to solve the white cross and the white-red-blue F2L pair in the least number of moves.\n");
  printf("Press 'Enter' to show the optimal solution and advance to the next scramble.\n\n");

  for (;;) {
    locdir_reset_xcross(&ldc);
    cube = to_cube(&ldc);
    sequence s = make_scramble(&cube, 9 + rand() % 5);
    locdir_apply_sequence(&ldc, s);
    unsigned char depth = nibble_depth(&tablebase, &ldc);

    printf("Solve in %d moves\n", depth);
    print_sequence(s);
    Cube cube = to_cube(&ldc);
    render(&cube);

    while (getchar() != '\n');
    locdir_reset(&ldc);
    locdir_apply_sequence(&ldc, s);
    sequence solution = nibble_solve(&tablebase, &ldc, &is_better_semistable);
    print_sequence(solution);
    printf("\n");
  }

  free_nibblebase(&tablebase);
}