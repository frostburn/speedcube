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

  Nibblebase tablebase = init_nibblebase(LOCDIR_CROSS_INDEX_SPACE, &locdir_cross_index);
  LocDirCube ldc;
  locdir_reset_cross(&ldc);
  populate_nibblebase(&tablebase, &ldc);

  Cube cube;

  printf("Hold the cube with the green face towards you and the white face pointing down and apply the given sequence.\n");
  printf("The objective is to solve the white cross in the least number of moves.\n");
  printf("Press 'Enter' to show the optimal solution and advance to the next scramble.\n\n");

  for (;;) {
    locdir_reset_cross(&ldc);
    cube = to_cube(&ldc);
    sequence s = make_scramble(&cube, 8 + rand() % 4);
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
}