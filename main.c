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

void oll_solutions() {
  /*
  FILE *fptr;
  printf("Loading database for the last 6 OLL moves.\n");
  GoalSphere sphere;
  sphere.hash_func = locdir_oll_index;
  sphere.num_sets = 6 + 1;
  sphere.sets = malloc(sphere.num_sets * sizeof(size_t*));
  sphere.set_sizes = malloc(sphere.num_sets * sizeof(size_t));
  sphere.set_sizes[0] = 1;
  sphere.set_sizes[1] = 21;
  sphere.set_sizes[2] = 387;
  sphere.set_sizes[3] = 7077;
  sphere.set_sizes[4] = 126006;
  sphere.set_sizes[5] = 2210527;
  sphere.set_sizes[6] = 38327451;
  fptr = fopen("./tables/oll_sphere.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < sphere.num_sets; ++i) {
    sphere.sets[i] = malloc(sphere.set_sizes[i] * sizeof(size_t));
    size_t num_read = fread(sphere.sets[i], sizeof(size_t), sphere.set_sizes[i], fptr);
    if (num_read != sphere.set_sizes[i]) {
      fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, sphere.set_sizes[i]);
      exit(EXIT_FAILURE);
    }
  }
  fclose(fptr);
  */

  LocDirCube root;

  printf("Generating an OLL goal sphere of radius 6.\n");
  locdir_reset(&root);
  GoalSphere sphere = init_goalsphere(&root, 6, &locdir_oll_index);
  printf("Done\n");

  LocDirCube *cases = malloc(216 * sizeof(LocDirCube));
  size_t *witnesses = malloc(216 * sizeof(size_t));
  Cube *replicas = malloc(216 * sizeof(Cube));
  size_t num_cases = 0;

  void generate(LocDirCube *ldc, Cube *replica, size_t depth) {
    Cube cube = to_cube(ldc);
    if (is_yellow_layer(&cube)) {
      size_t index = locdir_oll_index(ldc);
      for (size_t i = 0; i < num_cases; ++i) {
        if (index == witnesses[i]) {
          return;
        }
      }
      witnesses[num_cases] = index;
      replicas[num_cases] = *replica;
      cases[num_cases++] = *ldc;
    }
    if (depth <= 0) {
      return;
    }

    LocDirCube child = *ldc;
    locdir_U(&child);
    Cube child_replica = *replica;
    turn_U(&child_replica);
    generate(&child, &child_replica, depth - 1);

    child = *ldc;
    locdir_F(&child);
    locdir_R(&child);
    locdir_U(&child);
    locdir_R_prime(&child);
    locdir_U_prime(&child);
    locdir_F_prime(&child);
    child_replica = *replica;
    turn_F(&child_replica);
    turn_R(&child_replica);
    turn_U(&child_replica);
    turn_R_prime(&child_replica);
    turn_U_prime(&child_replica);
    turn_F_prime(&child_replica);
    generate(&child, &child_replica, depth - 1);
  }

  locdir_reset(&root);
  Cube root_replica;
  reset_oll(&root_replica);
  generate(&root, &root_replica, 140);

  printf("%zu OLL cases generated\n", num_cases);

  unsigned char search_depth = 4;
  size_t num_solvable = 0;
  for (size_t i = 0; i < num_cases; ++i) {
    sequence* solutions = goalsphere_solve_all(&sphere, cases + i, search_depth);
    if (solutions == NULL) {
      printf("Not solvable in %zu moves or less.\n", sphere.num_sets - 1 + search_depth);
    } else {
      num_solvable++;
      size_t j = 0;
      for (;;) {
        sequence solution = solutions[j];
        if (solution == SENTINEL) {
          break;
        }
        if (j < 4) {
          print_sequence(solutions[j]);
        }
        j++;
      }
      printf("\n%zu solutions of length %d\n", j, sequence_length(solutions[0]));
      free(solutions);
    }
    rotate_x_prime(replicas + i);
    render(replicas + i);
    printf("\n");
  }

  printf("%zu of %zu are solvable with given resources.\n", num_solvable, num_cases);

  free(cases);
  free(witnesses);
  free(replicas);
  free_goalsphere(&sphere);
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
