#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"

#include "cube.c"
#include "moves.c"
#include "sequence.c"
#include "locdir.c"
#include "tablebase.c"
#include "goalsphere.c"
#include "ida_star.c"
#include "global_solver.c"


void solve_2x2x2() {
  LocDirCube two_cubed;

  locdir_reset(&two_cubed);

  Nibblebase tablebase = init_nibblebase(LOCDIR_CORNER_INDEX_SPACE, &locdir_corner_index);

  populate_nibblebase(&tablebase, &two_cubed);

  for (int i = 0; i < 20; ++i) {
    locdir_scramble(&two_cubed);
    print_sequence(nibble_solve(&tablebase, &two_cubed, &is_better));
    Cube cube = to_cube(&two_cubed);
    render(&cube);
    printf("\n");
  }

  free_nibblebase(&tablebase);
}

void solve_3x3x3() {
  prepare_global_solver();

  LocDirCube ldc;
  printf("Solving a few easy scrambles...\n");
  for (size_t j = 0; j < 10; j++) {
    // locdir_scramble(&ldc);
    locdir_reset(&ldc);
    for (size_t i = 0; i < 15; ++i) {
      locdir_apply_stable(&ldc, STABLE_MOVES[rand() % NUM_STABLE_MOVES]);
    }

    Cube cube = to_cube(&ldc);
    render(&cube);

    sequence solution = global_solve(&ldc);

    printf("Found a solution in %d moves:\n", sequence_length(solution));

    print_sequence(solution);
    apply_sequence(&cube, solution);
    printf("Becomes:\n");
    render(&cube);
    printf("\n");
  }

  printf("Collecting statistics...\n");

  clock_t start = clock();
  size_t total_solves = 1;
  size_t total_moves = 0;
  size_t min_moves = ~0ULL;
  size_t max_moves = 0;
  for (size_t i = 0; i < total_solves; ++i) {
    locdir_reset(&ldc);
    locdir_scramble(&ldc);

    sequence solution = global_solve(&ldc);
    size_t num_moves = sequence_length(solution);
    total_moves += num_moves;
    if (num_moves < min_moves) {
      min_moves = num_moves;
    }
    if (num_moves > max_moves) {
      max_moves = num_moves;
    }
  }
  clock_t end = clock();

  double took = end - start;
  took /= CLOCKS_PER_SEC;
  took /= 60 * 60;

  printf("Solved %zu scrambles in %g hours (%g hours / solution).\n", total_solves, took, took/total_solves);
  printf("Minimum number of moves in a solution = %zu\n", min_moves);
  printf("Average number of moves in a solution = %g\n", ((double)total_moves) / total_solves);
  printf("Maximum number of moves in a solution = %zu\n", max_moves);

  free_global_solver();
}


void pll_solutions() {
  prepare_global_solver();

  LocDirCube root;

  LocDirCube *cases = malloc(288 * sizeof(LocDirCube));
  size_t num_cases = 0;

  sequence Ga = parse("R2 U R' U R' U' R U' R2 U' D R' U R D'");

  void generate(LocDirCube *ldc, size_t depth) {
    Cube cube = to_cube(ldc);
    if (is_yellow_permutation(&cube)) {
      for (size_t i = 0; i < num_cases; ++i) {
        if (locdir_equals(ldc, cases + i)) {
          return;
        }
      }
      cases[num_cases++] = *ldc;
    }
    if (depth <= 0) {
      return;
    }

    LocDirCube child = *ldc;
    locdir_U(&child);
    generate(&child, depth - 1);

    child = *ldc;
    locdir_F(&child);
    locdir_R(&child);
    locdir_U(&child);
    locdir_R_prime(&child);
    locdir_U_prime(&child);
    locdir_F_prime(&child);
    generate(&child, depth - 1);

    child = *ldc;
    locdir_apply_sequence(&child, Ga);
    generate(&child, depth - 1);
  }

  locdir_reset(&root);
  generate(&root, 14);

  printf("%zu PLL cases generated\n", num_cases);

  unsigned char search_depth = 2;
  size_t num_solvable = 0;
  for (size_t i = 0; i < num_cases; ++i) {
    Cube cube = to_cube(cases + i);
    rotate_x_prime(&cube);
    sequence solution = goalsphere_solve(&GLOBAL_SOLVER.goal, cases + i, search_depth, &is_better);
    if (solution == INVALID) {
      printf("Not solvable in %zu moves or less. Switching to IDA*...\n", GLOBAL_SOLVER.goal.num_sets - 1 + search_depth);

      solution = global_solve(cases + i);
      printf("Solved in %d moves\n", sequence_length(solution));

      print_sequence(solution);
    } else {
      num_solvable++;
      int depth = sequence_length(solution);
      if (depth == 1) {
        printf("Solvable in 1 move!\n");
      } else {
        printf("Solvable in %d moves!\n", depth);
      }
      print_sequence(solution);
    }
    render(&cube);
    printf("\n");
  }

  printf("Done\n");
  printf("%zu of %zu were solved with the most comfortable solution.\n", num_solvable, num_cases);

  free(cases);
  free_global_solver();
}

void cross_trainer() {
  Nibblebase tablebase = init_nibblebase(LOCDIR_CROSS_INDEX_SPACE, &locdir_cross_index);
  LocDirCube ldc;
  locdir_reset_cross(&ldc);
  populate_nibblebase(&tablebase, &ldc);

  Cube cube;

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


void xcross_trainer() {
  FILE *fptr;
  size_t num_read;
  size_t tablebase_size;

  fprintf(stderr, "Loading tablebase for xcross.\n");
  Nibblebase tablebase = init_nibblebase(LOCDIR_XCROSS_INDEX_SPACE, &locdir_xcross_index);
  #if SCISSORS_ENABLED
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

void cross_stats() {
  Nibblebase tablebase = init_nibblebase(LOCDIR_CROSS_INDEX_SPACE, &locdir_cross_index);
  LocDirCube ldc;
  locdir_reset_cross(&ldc);
  populate_nibblebase(&tablebase, &ldc);

  size_t depths[9] = {0};

  size_t N = 100000;

  printf("=== Single ===\n");
  for (size_t i = 0; i < N; ++i) {
    locdir_reset(&ldc);
    locdir_scramble(&ldc);

    unsigned char depth = nibble_depth(&tablebase, &ldc);
    depths[depth]++;
  }

  double average = 0;
  for (size_t i = 0; i < 9; ++i) {
    printf("%zu: ", i);
    double portion = ((double)depths[i]) / N;
    for (size_t j = 0; j < portion * 100; ++j) {
      printf("#");
    }
    printf(" %g%%\n", portion * 100);
    average += i * portion;
    depths[i] = 0;
  }
  printf("Average = %g\n", average);

  printf("\n=== Double ===\n");

  for (size_t i = 0; i < N; ++i) {
    int seed = rand();
    srand(seed);
    locdir_reset(&ldc);
    locdir_scramble(&ldc);
    unsigned char white_depth = nibble_depth(&tablebase, &ldc);

    srand(seed);
    locdir_reset(&ldc);
    locdir_z_prime(&ldc);
    locdir_scramble(&ldc);
    locdir_z(&ldc);
    unsigned char orange_depth = nibble_depth(&tablebase, &ldc);

    unsigned char depth = orange_depth < white_depth ? orange_depth : white_depth;

    depths[depth]++;
  }

  average = 0;
  for (size_t i = 0; i < 9; ++i) {
    printf("%zu: ", i);
    double portion = ((double)depths[i]) / N;
    for (size_t j = 0; j < portion * 100; ++j) {
      printf("#");
    }
    printf(" %g%%\n", portion * 100);
    average += i * portion;
    depths[i] = 0;
  }
  printf("Average = %g\n", average);

  printf("\n=== Neutral ===\n");

  for (size_t i = 0; i < N; ++i) {
    int seed = rand();
    srand(seed);
    locdir_reset(&ldc);
    locdir_scramble(&ldc);
    unsigned char white_depth = nibble_depth(&tablebase, &ldc);

    srand(seed);
    locdir_reset(&ldc);
    locdir_z_prime(&ldc);
    locdir_scramble(&ldc);
    locdir_z(&ldc);
    unsigned char orange_depth = nibble_depth(&tablebase, &ldc);

    srand(seed);
    locdir_reset(&ldc);
    locdir_z2(&ldc);
    locdir_scramble(&ldc);
    locdir_z2(&ldc);
    unsigned char yellow_depth = nibble_depth(&tablebase, &ldc);

    srand(seed);
    locdir_reset(&ldc);
    locdir_z(&ldc);
    locdir_scramble(&ldc);
    locdir_z_prime(&ldc);
    unsigned char red_depth = nibble_depth(&tablebase, &ldc);

    srand(seed);
    locdir_reset(&ldc);
    locdir_x_prime(&ldc);
    locdir_scramble(&ldc);
    locdir_x(&ldc);
    unsigned char blue_depth = nibble_depth(&tablebase, &ldc);

    srand(seed);
    locdir_reset(&ldc);
    locdir_x(&ldc);
    locdir_scramble(&ldc);
    locdir_x_prime(&ldc);
    unsigned char green_depth = nibble_depth(&tablebase, &ldc);

    unsigned char depth = orange_depth < white_depth ? orange_depth : white_depth;
    depth = depth < yellow_depth ? depth : yellow_depth;
    depth = depth < red_depth ? depth : red_depth;
    depth = depth < blue_depth ? depth : blue_depth;
    depth = depth < green_depth ? depth : green_depth;

    depths[depth]++;
  }

  average = 0;
  for (size_t i = 0; i < 9; ++i) {
    printf("%zu: ", i);
    double portion = ((double)depths[i]) / N;
    for (size_t j = 0; j < portion * 100; ++j) {
      printf("#");
    }
    printf(" %g%%\n", portion * 100);
    average += i * portion;
    depths[i] = 0;
  }
  printf("Average = %g\n", average);
}

void xcross_stats() {
  FILE *fptr;
  size_t num_read;
  size_t tablebase_size;

  fprintf(stderr, "Loading tablebase for xcross.\n");
  Nibblebase tablebase = init_nibblebase(LOCDIR_XCROSS_INDEX_SPACE, &locdir_xcross_index);
  #if SCISSORS_ENABLED
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

  size_t depths[10] = {0};

  size_t M = sizeof(depths) / sizeof(size_t);

  size_t N = 100000;

  printf("=== Single ===\n");
  for (size_t i = 0; i < N; ++i) {
    locdir_reset(&ldc);
    locdir_scramble(&ldc);

    unsigned char depth = nibble_depth(&tablebase, &ldc);
    depths[depth]++;
  }

  double average = 0;
  for (size_t i = 0; i < M; ++i) {
    printf("%zu: ", i);
    double portion = ((double)depths[i]) / N;
    for (size_t j = 0; j < portion * 100; ++j) {
      printf("#");
    }
    printf(" %g%%\n", portion * 100);
    average += i * portion;
    depths[i] = 0;
  }
  printf("Average = %g\n", average);

  printf("\n=== Single (neutral pair) ===\n");

  for (size_t i = 0; i < N; ++i) {
    int seed = rand();
    srand(seed);
    locdir_reset(&ldc);
    locdir_scramble(&ldc);
    unsigned char depth = nibble_depth(&tablebase, &ldc);

    srand(seed);
    locdir_reset(&ldc);
    locdir_y(&ldc);
    locdir_scramble(&ldc);
    locdir_y_prime(&ldc);
    unsigned char alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_y2(&ldc);
    locdir_scramble(&ldc);
    locdir_y2(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_y_prime(&ldc);
    locdir_scramble(&ldc);
    locdir_y(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    depths[depth]++;
  }

  average = 0;
  for (size_t i = 0; i < M; ++i) {
    printf("%zu: ", i);
    double portion = ((double)depths[i]) / N;
    for (size_t j = 0; j < portion * 100; ++j) {
      printf("#");
    }
    printf(" %g%%\n", portion * 100);
    average += i * portion;
    depths[i] = 0;
  }
  printf("Average = %g\n", average);

  printf("\n=== True neutral ===\n");

  for (size_t i = 0; i < N; ++i) {
    // White
    int seed = rand();
    srand(seed);
    locdir_reset(&ldc);
    locdir_scramble(&ldc);
    unsigned char depth = nibble_depth(&tablebase, &ldc);

    srand(seed);
    locdir_reset(&ldc);
    locdir_y(&ldc);
    locdir_scramble(&ldc);
    locdir_y_prime(&ldc);
    unsigned char alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_y2(&ldc);
    locdir_scramble(&ldc);
    locdir_y2(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_y_prime(&ldc);
    locdir_scramble(&ldc);
    locdir_y(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    // Green
    srand(seed);
    locdir_reset(&ldc);
    locdir_x_prime(&ldc);
    locdir_scramble(&ldc);
    locdir_x(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_x_prime(&ldc);
    locdir_y(&ldc);
    locdir_scramble(&ldc);
    locdir_y_prime(&ldc);
    locdir_x(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_x_prime(&ldc);
    locdir_y2(&ldc);
    locdir_scramble(&ldc);
    locdir_y2(&ldc);
    locdir_x(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_x_prime(&ldc);
    locdir_y_prime(&ldc);
    locdir_scramble(&ldc);
    locdir_y(&ldc);
    locdir_x(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    // Blue
    srand(seed);
    locdir_reset(&ldc);
    locdir_x(&ldc);
    locdir_scramble(&ldc);
    locdir_x_prime(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_x(&ldc);
    locdir_y(&ldc);
    locdir_scramble(&ldc);
    locdir_y_prime(&ldc);
    locdir_x_prime(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_x(&ldc);
    locdir_y2(&ldc);
    locdir_scramble(&ldc);
    locdir_y2(&ldc);
    locdir_x_prime(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_x(&ldc);
    locdir_y_prime(&ldc);
    locdir_scramble(&ldc);
    locdir_y(&ldc);
    locdir_x_prime(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    // Orange
    srand(seed);
    locdir_reset(&ldc);
    locdir_z_prime(&ldc);
    locdir_scramble(&ldc);
    locdir_z(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_z_prime(&ldc);
    locdir_y(&ldc);
    locdir_scramble(&ldc);
    locdir_y_prime(&ldc);
    locdir_z(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_z_prime(&ldc);
    locdir_y2(&ldc);
    locdir_scramble(&ldc);
    locdir_y2(&ldc);
    locdir_z(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_z_prime(&ldc);
    locdir_y_prime(&ldc);
    locdir_scramble(&ldc);
    locdir_y(&ldc);
    locdir_z(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    // Red
    srand(seed);
    locdir_reset(&ldc);
    locdir_z(&ldc);
    locdir_scramble(&ldc);
    locdir_z_prime(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_z(&ldc);
    locdir_y(&ldc);
    locdir_scramble(&ldc);
    locdir_y_prime(&ldc);
    locdir_z_prime(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_z(&ldc);
    locdir_y2(&ldc);
    locdir_scramble(&ldc);
    locdir_y2(&ldc);
    locdir_z_prime(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_z(&ldc);
    locdir_y_prime(&ldc);
    locdir_scramble(&ldc);
    locdir_y(&ldc);
    locdir_z_prime(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    // Yellow
    srand(seed);
    locdir_reset(&ldc);
    locdir_x2(&ldc);
    locdir_scramble(&ldc);
    locdir_x2(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_x2(&ldc);
    locdir_y(&ldc);
    locdir_scramble(&ldc);
    locdir_y_prime(&ldc);
    locdir_x2(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_x2(&ldc);
    locdir_y2(&ldc);
    locdir_scramble(&ldc);
    locdir_y2(&ldc);
    locdir_x2(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    srand(seed);
    locdir_reset(&ldc);
    locdir_x2(&ldc);
    locdir_y_prime(&ldc);
    locdir_scramble(&ldc);
    locdir_y(&ldc);
    locdir_x2(&ldc);
    alt = nibble_depth(&tablebase, &ldc);
    depth = depth < alt ? depth : alt;

    depths[depth]++;
  }

  average = 0;
  for (size_t i = 0; i < M; ++i) {
    printf("%zu: ", i);
    double portion = ((double)depths[i]) / N;
    for (size_t j = 0; j < portion * 100; ++j) {
      printf("#");
    }
    printf(" %g%%\n", portion * 100);
    average += i * portion;
    depths[i] = 0;
  }
  printf("Average = %g\n", average);
}

void solve_f2l_pair() {
  FILE *fptr;
  size_t num_read;
  size_t tablebase_size;

  fprintf(stderr, "Loading tablebase for xcross.\n");
  Nibblebase tablebase = init_nibblebase(LOCDIR_XCROSS_INDEX_SPACE, &locdir_xcross_index);
  #if SCISSORS_ENABLED
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


  printf("Generating cases...\n");
  LocDirCube root;

  LocDirCube *cases = malloc(384 * sizeof(LocDirCube));
  size_t num_cases = 0;

  locdir_reset_xcross(&root);
  for (int i = 0; i < 6; ++i) {
    root.center_locs[i] = i;
  }
  for (int i = 0; i < 8; ++i) {
    root.corner_locs[4] = i;
    for (int j = 0; j < 3; ++j) {
      root.corner_dirs[4] = j;
      for (int k = 0; k < 8; ++k) {
        root.edge_locs[4] = k;
        for (int l = 0; l < 2; ++l) {
          root.edge_dirs[4] = l;
          cases[num_cases++] = root;
        }
      }
    }
  }

  printf("%zu cases generated\n", num_cases);

  sequence *solutions = malloc(384 * sizeof(sequence));
  size_t num_solutions;
  int max_length = 0;

  for (size_t i = 0; i < num_cases; ++i) {
    solutions[num_solutions++] = nibble_solve(&tablebase, cases + i, &is_better_semistable);
    int length = sequence_length(solutions[num_solutions - 1]);
    max_length = max_length > length ? max_length : length;
  }

  for (int l = 0; l <= max_length; ++l) {
    printf("Optimal solutions of length %d:\n", l);
    for (size_t i = 0; i < num_cases; ++i) {
      if (sequence_length(solutions[i]) != l) {
        continue;
      }
      Cube cube = to_cube(cases + i);
      rotate_x_prime(&cube);
      print_sequence(solutions[i]);
      render(&cube);
      printf("\n");
    }
  }

  free(cases);
  free(solutions);
  free_nibblebase(&tablebase);
}

int main() {
  srand(time(NULL));

  // solve_2x2x2();

  // solve_3x3x3();

  // pll_solutions();

  // oll_solutions();

  // cross_trainer();

  // xcross_trainer();

  xcross_stats();

  // solve_f2l_pair();

  return EXIT_SUCCESS;
}
