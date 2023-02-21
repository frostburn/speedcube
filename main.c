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
    print_sequence(nibble_solve(&tablebase, &two_cubed));
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
    for (size_t i = 0; i < 13; ++i) {
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
  size_t total_solves = 2;
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

  FILE *fptr;

  printf("Loading database for the last 6 moves.\n");
  GoalSphere sphere;
  sphere.hash_func = locdir_centerless_hash;
  sphere.num_sets = 6 + 1;
  sphere.sets = malloc(sphere.num_sets * sizeof(size_t*));
  sphere.set_sizes = malloc(sphere.num_sets * sizeof(size_t));
  sphere.set_sizes[0] = 1;
  sphere.set_sizes[1] = 27;
  sphere.set_sizes[2] = 501;
  sphere.set_sizes[3] = 9175;
  sphere.set_sizes[4] = 164900;
  sphere.set_sizes[5] = 2912447;
  sphere.set_sizes[6] = 50839041;
  fptr = fopen("./tables/centerless_sphere.bin", "rb");
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

  LocDirCube root;

  /*
  printf("Generating a goal sphere of radius 7\n");
  locdir_reset(&root);
  GoalSphere sphere = init_goalsphere(&root, 7, &locdir_centerless_hash);
  printf("Done\n");
  */

  LocDirCube *cases = malloc(96 * sizeof(LocDirCube));
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
    sequence solution = goalsphere_solve(&sphere, cases + i, search_depth);
    if (solution == INVALID) {
      printf("Not solvable in %zu moves or less. Switching to IDA*...\n", sphere.num_sets - 1 + search_depth);

      ida_star_solve(&GLOBAL_SOLVER.ida, cases + i, 0);

      printf("Found a solution in %zu moves:\n", GLOBAL_SOLVER.ida.path_length - 1);

      solution = ida_to_sequence(&GLOBAL_SOLVER.ida);
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
  free_goalsphere(&sphere);
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

int main() {
  srand(time(NULL));

  // solve_2x2x2();

  // solve_3x3x3();

  // pll_solutions();

  oll_solutions();

  return EXIT_SUCCESS;
}
