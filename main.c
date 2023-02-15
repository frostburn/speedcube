#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"

#include "cube.c"
#include "moves.c"
#include "sequence.c"
#include "bst.c"
#include "indexer.c"
#include "locdir.c"
#include "tablebase.c"
#include "goalsphere.c"
#include "ida_star.c"

typedef struct {
  Cube *cubes;
  sequence *sequences;
  Node *nodes;
  Node *root;
  size_t size;
  size_t max_size;
} Database;


/* Utilities */

void scramble(Cube *cube) {
  for (int i = 0; i < 100; ++i) {
    int r = rand() % 6;
    switch(r) {
      case 0:
        turn_U(cube);
        break;
      case 1:
        turn_D(cube);
        break;
      case 2:
        turn_R(cube);
        break;
      case 3:
        turn_L(cube);
        break;
      case 4:
        turn_F(cube);
        break;
      case 5:
        turn_B(cube);
        break;
    }
  }
}

void roll(Cube *cube) {
  for (int i = 0; i < 100; ++i) {
    int r = rand() % 2;
    switch(r) {
      case 0:
        rotate_y_prime(cube);
        break;
      case 1:
        rotate_x(cube);
        break;
    }
  }
}

bool is_last_layer(Cube *cube) {
  if (is_top_layer(cube)) {
    return true;
  }

  Cube clone = *cube;
  rotate_x(&clone);
  if (is_top_layer(&clone)) {
    return true;
  }
  rotate_x(&clone);
  if (is_top_layer(&clone)) {
    return true;
  }
  rotate_x(&clone);
  if (is_top_layer(&clone)) {
    return true;
  }

  rotate_y_prime(&clone);
  rotate_x(&clone);
  if (is_top_layer(&clone)) {
    return true;
  }
  rotate_x(&clone);
  rotate_x(&clone);
  if (is_top_layer(&clone)) {
    return true;
  }
  return false;
}

Database init_database(size_t max_size) {
  Database result;
  result.max_size = max_size;
  result.size = 0;
  result.cubes = malloc(max_size * sizeof(Cube));
  result.sequences = malloc(max_size * sizeof(sequence));
  result.nodes = calloc(max_size, sizeof(Node));
  result.root = NULL;
  return result;
}

void free_database(Database *database) {
  free(database->cubes);
  free(database->sequences);
  free(database->nodes);
}

bool update(Database *database, Cube *cube, sequence seq) {
  Cube *existing = get(database->root, cube);
  if (existing != NULL) {
    size_t i = existing - database->cubes;
    if (is_better(seq, database->sequences[i])) {
      database->sequences[i] = seq;
      return true;
    }
    return false;
  }
  if (database->size >= database->max_size) {
    return false;
  }
  database->cubes[database->size].a = cube->a;
  database->cubes[database->size].b = cube->b;
  database->cubes[database->size].c = cube->c;
  database->sequences[database->size] = seq;
  database->nodes[database->size].cube = database->cubes + database->size;
  if (database->size) {
    insert(database->root, database->nodes + database->size);
  } else {
    database->root = database->nodes;
    database->root->cube = database->cubes;
  }
  database->size++;
  return true;
}

void update_variants(Database *database, Cube *cube, sequence seq) {
  for (sequence m = U; m <= MAX_MOVE; ++m) {
    Cube variant = *cube;
    sequence var_seq = NUM_MOVES * seq + m;
    apply(&variant, m);
    update(database, &variant, var_seq);
  }
}

void fill_database(Database *database, Cube *cube) {
  size_t low = -1;
  size_t high = 0;
  update(database, cube, 0);
  while (low != high) {
    low = high;
    high = database->size;
    for (size_t i = low; i < high; ++i) {
      update_variants(database, database->cubes + i, database->sequences[i]);
    }
    database->root = balance(database->nodes, database->size);
  }
}

void update_subgroup_variants(Database *database, Cube *cube, sequence seq, sequence *sequences, size_t num_sequences) {
  for (size_t i = 0; i < num_sequences; ++i) {
    Cube variant = *cube;
    sequence var_seq = concat(seq, sequences[i]);
    apply_sequence(&variant, sequences[i]);
    update(database, &variant, var_seq);
  }
}

void fill_subgroup(Database *database, Cube *cube, sequence *sequences, size_t num_sequences) {
  size_t low = -1;
  size_t high = 0;
  update(database, cube, 0);
  while (low != high) {
    low = high;
    high = database->size;
    for (size_t i = low; i < high; ++i) {
      update_subgroup_variants(database, database->cubes + i, database->sequences[i], sequences, num_sequences);
    }
    database->root = balance(database->nodes, database->size);
  }
}

Database generate_yellow_subgroup() {
  Database database = init_database(20736);
  sequence sequences[] = {
    U,
    from_moves((enum move[]){F, R, U, R_prime, U_prime, F_prime, I}),
  };
  Cube cube;
  reset(&cube);
  fill_subgroup(&database, &cube, sequences, 2);
  return database;
}

Database generate_oll_subgroup() {
  Database database = init_database(216);
  sequence sequences[] = {
    U,
    from_moves((enum move[]){F, R, U, R_prime, U_prime, F_prime, I}),
  };
  Cube cube;
  reset_oll(&cube);
  fill_subgroup(&database, &cube, sequences, 2);
  return database;
}

Database generate_cross_subgroup() {
  Database database = init_database(190080);
  sequence sequences[] = {U, D, R, L, F, B};
  Cube cube;
  reset_cross(&cube);
  fill_subgroup(&database, &cube, sequences, 6);
  return database;
}

Database generate_f2l_subgroup() {
  Database database = init_database(9525600);
  sequence sequences[] = {
    U,
    from_moves((enum move[]){R, U, R_prime, I}),
    from_moves((enum move[]){L, U, L_prime, I}),
    from_moves((enum move[]){B, U, B_prime, I}),
    from_moves((enum move[]){B_prime, U, B, I}),
  };
  Cube cube;
  reset_f2l(&cube);
  fill_subgroup(&database, &cube, sequences, 5);
  return database;
}

sequence solve_cacheless(Database *database, Cube *cube, sequence seq, size_t depth) {
  Cube *scramble = get(database->root, cube);
  if (scramble != NULL) {
    size_t index = scramble - database->cubes;
    return concat(seq, invert(database->sequences[index]));
  }
  if (depth <= 0) {
    return INVALID;
  }

  sequence solution = INVALID;
  for (sequence m = U; m <= MAX_MOVE; ++m) {
    Cube variant = *cube;
    apply(&variant, m);
    sequence var_seq = seq * NUM_MOVES + m;
    sequence candidate = solve_cacheless(database, &variant, var_seq, depth - 1);
    if (is_better(candidate, solution)) {
      solution = candidate;
    }
  }
  return solution;
}

/* Database has scrambles not solutions. */
sequence solve(Database *database, Cube *cube, size_t cache_size, size_t depth) {
  Database cache = init_database(cache_size);
  size_t low = -1;
  size_t high = 0;
  update(&cache, cube, 0);

  sequence solution = INVALID;

  while (low != high && solution == INVALID) {
    low = high;
    high = cache.size;
    for (size_t i = low; i < high; ++i) {
      Cube *scramble = get(database->root, cache.cubes + i);
      if (scramble != NULL) {
        size_t index = scramble - database->cubes;
        sequence candidate = concat(cache.sequences[i], invert(database->sequences[index]));
        if (is_better(candidate, solution)) {
          solution = candidate;
        }
      }
      for (sequence m = U; m <= MAX_MOVE; ++m) {
        Cube variant = cache.cubes[i];
        sequence var_seq = NUM_MOVES * cache.sequences[i] + m;
        apply(&variant, m);
        if (cache.size < cache.max_size) {
          update(&cache, &variant, var_seq);
        } else {
          solve_cacheless(database, &variant, var_seq, depth);
        }
      }
    }
    cache.root = balance(cache.nodes, cache.size);
  }

  free_database(&cache);

  return solution;
}

void solve_ll(size_t database_size, size_t cache_size, size_t depth, bool oll) {
  printf("Generating yellow subgroup...\n");
  Database subgroup;
  if (oll) {
    printf("...OLL only.\n");
    subgroup = generate_oll_subgroup();
  } else {
    subgroup = generate_yellow_subgroup();
  }

  printf("Generating scramble database...\n");
  Cube cube;
  if (oll) {
    reset_oll(&cube);
  } else {
    reset(&cube);
  }
  Database database = init_database(database_size);

  fill_database(&database, &cube);

  size_t num_permutations = 0;
  size_t num_solutions = 0;

  printf("Solving...\n");

  for (size_t i = 0; i < subgroup.size; ++i) {
    if (!is_yellow_layer(subgroup.cubes + i)) {
      fprintf(stderr, "Inconsistent element found\n");
      render(subgroup.cubes + i);
      exit(EXIT_FAILURE);
    }
    if (oll || is_yellow_permutation(subgroup.cubes + i)) {
      sequence solution = solve(&database, subgroup.cubes + i, cache_size, depth);
      if (solution != INVALID) {
        num_solutions++;
      }
      print_sequence(solution);
      Cube front_view = subgroup.cubes[i];
      rotate_x_prime(&front_view);
      render(&front_view);
      printf("\n");
      num_permutations++;
    }
  }

  printf("Done with %zu elements", subgroup.size);
  if(!oll) {
    printf(" of which %zu are permutations", num_permutations);
  }
  printf(". Solved %zu of them.\n", num_solutions);

  free_database(&subgroup);
  free_database(&database);
}

void solve_cross() {
  Cube cube;
  reset_cross(&cube);
  printf("Initializing indexer...\n");
  Indexer indexer = init_indexer(190080);
  printf("Filling indexer...\n");
  fill_indexer(&indexer, &cube);
  printf("Solving the cross on random scrambles...\n");

  for (size_t i = 0; i < 20; ++i) {
    reset_cross(&cube);
    scramble(&cube);
    roll(&cube);
    sequence solution = indexed_solution(&indexer, &cube);
    print_sequence(solution);
    render(&cube);
    printf("Becomes:\n");
    apply_sequence(&cube, solution);
    render(&cube);
    printf("\n");
  }

  free_indexer(&indexer);
}

void solve_f2l(size_t database_size, size_t cache_size, size_t depth, size_t sample_size) {
  printf("Generating F2L subgroup...\n");
  Database subgroup = generate_f2l_subgroup();

  printf("Generating scramble database...\n");
  Cube cube;
  reset_f2l(&cube);
  Database database = init_database(database_size);

  fill_database(&database, &cube);

  size_t num_solutions = 0;
  int max_length = 0;

  printf("Solving a sample of %zu...\n", sample_size);

  for (size_t j = 0; j < sample_size; ++j) {
    size_t i = rand() % subgroup.size;
    sequence solution = solve(&database, subgroup.cubes + i, cache_size, depth);
    int length = sequence_length(solution);
    if (solution != INVALID) {
      num_solutions++;
      max_length = length > max_length ? length : max_length;
    }
    if (length >= 7) {
      print_sequence(solution);
      Cube front_view = subgroup.cubes[i];
      rotate_x(&front_view);
      render(&front_view);
      printf("\n");
    }
  }

  double success_rate = num_solutions * 100;
  success_rate /= sample_size;

  printf("Done with %zu elements. Solved %zu of them with success rate %g%%. Longest solution took %d moves.\n", subgroup.size, num_solutions, success_rate, max_length);

  free_database(&subgroup);
  free_database(&database);
}

void solve_2x2x2() {
  LocDirCube two_cubed;

  locdir_reset(&two_cubed);

  // Nibblebase tablebase = init_nibblebase(LOCDIR_FOUR_CORNER_INDEX_SPACE, &locdir_four_corner_index);
  Nibblebase tablebase = init_nibblebase(LOCDIR_CORNER_INDEX_SPACE, &locdir_corner_index);

  populate_nibblebase(&tablebase, &two_cubed);

  for (int i = 0; i < 20; ++i) {
    locdir_scramble(&two_cubed);
    print_sequence(nibble_solution(&tablebase, &two_cubed));
    Cube cube = to_cube(&two_cubed);
    render(&cube);
    printf("\n");
  }

  free_nibblebase(&tablebase);
}


void solve_edges() {
  FILE *fptr;
  size_t num_read;
  size_t tablebase_size;

  printf("Loading tablebase for first 7 edges.\n");
  Nibblebase first = init_nibblebase(LOCDIR_FIRST_7_EDGE_INDEX_SPACE, &locdir_first_7_edge_index);
  fptr = fopen("./tables/first_7_edges.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_FIRST_7_EDGE_INDEX_SPACE + 1)/2;
  num_read = fread(first.octets, sizeof(unsigned char), tablebase_size, fptr);
  if (num_read != tablebase_size) {
    fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, tablebase_size);
    exit(EXIT_FAILURE);
  }
  fclose(fptr);

  printf("Loading tablebase for last 7 edges.\n");
  Nibblebase last = init_nibblebase(LOCDIR_LAST_7_EDGE_INDEX_SPACE, &locdir_last_7_edge_index);
  fptr = fopen("./tables/last_7_edges.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_LAST_7_EDGE_INDEX_SPACE + 1)/2;
  num_read = fread(last.octets, sizeof(unsigned char), tablebase_size, fptr);
  if (num_read != tablebase_size) {
    fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, tablebase_size);
    exit(EXIT_FAILURE);
  }
  fclose(fptr);

  printf("Loading database for the last 6 moves of an edges-only cube.\n");
  GoalSphere edge_sphere;
  edge_sphere.hash_func = locdir_edge_index;
  edge_sphere.num_sets = 6 + 1;
  edge_sphere.sets = malloc(edge_sphere.num_sets * sizeof(size_t*));
  edge_sphere.set_sizes = malloc(edge_sphere.num_sets * sizeof(size_t));
  edge_sphere.set_sizes[0] = 1;
  edge_sphere.set_sizes[1] = 27;
  edge_sphere.set_sizes[2] = 501;
  edge_sphere.set_sizes[3] = 9121;
  edge_sphere.set_sizes[4] = 157886;
  edge_sphere.set_sizes[5] = 2612316;
  edge_sphere.set_sizes[6] = 41391832;
  fptr = fopen("./tables/edge_sphere.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < edge_sphere.num_sets; ++i) {
    edge_sphere.sets[i] = malloc(edge_sphere.set_sizes[i] * sizeof(size_t));
    num_read = fread(edge_sphere.sets[i], sizeof(size_t), edge_sphere.set_sizes[i], fptr);
    if (num_read != edge_sphere.set_sizes[i]) {
      fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, edge_sphere.set_sizes[i]);
      exit(EXIT_FAILURE);
    }
  }
  fclose(fptr);

  unsigned char estimator(LocDirCube *ldc) {
    unsigned char first_depth = get_nibble(&first, (*first.index_func)(ldc));
    unsigned char last_depth = get_nibble(&last, (*last.index_func)(ldc));
    unsigned char sphere_depth = goalsphere_depth(&edge_sphere, (*edge_sphere.hash_func)(ldc));
    // We know that the cube was not solvable in 6 moves or less, so we can estimate at 7.
    if (sphere_depth == UNKNOWN) {
      sphere_depth = edge_sphere.num_sets;
    }
    if (first_depth > last_depth && first_depth > sphere_depth) {
      return first_depth;
    }
    if (sphere_depth > last_depth) {
      return sphere_depth;
    }
    return last_depth;
  }

  IDAstar ida;
  ida.is_solved = locdir_edges_solved;
  ida.estimator = estimator;

  LocDirCube edges;
  locdir_reset_edges(&edges);


  printf("Solving a few scrambles...\n");
  for (size_t j = 0; j < 5; j++) {
    locdir_scramble(&edges);

    ida_star_solve(&ida, &edges);

    printf("Found a solution in %zu moves:\n", ida.path_length - 1);

    for (size_t i = 0; i < ida.path_length; ++i) {
      Cube cube = to_cube(ida.path + i);
      render(&cube);
    }
  }

  printf("Collecting statistics...\n");

  clock_t start = clock();
  size_t total_solves = 10000;
  size_t total_moves = 0;
  size_t min_moves = ~0ULL;
  size_t max_moves = 0;
  for (size_t i = 0; i < total_solves; ++i) {
    locdir_scramble(&edges);
    ida_star_solve(&ida, &edges);
    size_t num_moves = ida.path_length - 1;
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

  printf("Solved %zu scrambles in %g seconds (%g ms / solution).\n", total_solves, took, 1000 * took/total_solves);
  printf("Minimum number of moves in a solution = %zu\n", min_moves);
  printf("Average number of moves in a solution = %g\n", ((double)total_moves) / total_solves);
  printf("Maximum number of moves in a solution = %zu\n", max_moves);

  free_nibblebase(&first);
  free_nibblebase(&last);
  free_goalsphere(&edge_sphere);
}

void solve_3x3x3() {
  FILE *fptr;
  size_t num_read;
  size_t tablebase_size;

  printf("Loading tablebase for first 7 edges.\n");
  Nibblebase first = init_nibblebase(LOCDIR_FIRST_7_EDGE_INDEX_SPACE, &locdir_first_7_edge_index);
  fptr = fopen("./tables/first_7_edges.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_FIRST_7_EDGE_INDEX_SPACE + 1)/2;
  num_read = fread(first.octets, sizeof(unsigned char), tablebase_size, fptr);
  if (num_read != tablebase_size) {
    fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, tablebase_size);
    exit(EXIT_FAILURE);
  }
  fclose(fptr);

  printf("Loading tablebase for last 7 edges.\n");
  Nibblebase last = init_nibblebase(LOCDIR_LAST_7_EDGE_INDEX_SPACE, &locdir_last_7_edge_index);
  fptr = fopen("./tables/last_7_edges.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_LAST_7_EDGE_INDEX_SPACE + 1)/2;
  num_read = fread(last.octets, sizeof(unsigned char), tablebase_size, fptr);
  if (num_read != tablebase_size) {
    fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, tablebase_size);
    exit(EXIT_FAILURE);
  }
  fclose(fptr);

  printf("Loading tablebase for the corners.\n");
  Nibblebase corners = init_nibblebase(LOCDIR_CORNER_INDEX_SPACE, &locdir_corner_index);
  fptr = fopen("./tables/corners.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_CORNER_INDEX_SPACE + 1)/2;
  num_read = fread(corners.octets, sizeof(unsigned char), tablebase_size, fptr);
  if (num_read != tablebase_size) {
    fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, tablebase_size);
    exit(EXIT_FAILURE);
  }
  fclose(fptr);

  unsigned char estimator(LocDirCube *ldc) {
    unsigned char first_depth = get_nibble(&first, (*first.index_func)(ldc));
    unsigned char last_depth = get_nibble(&last, (*last.index_func)(ldc));
    unsigned char corners_depth = get_nibble(&corners, (*corners.index_func)(ldc));

    if (first_depth > last_depth && first_depth > corners_depth) {
      return first_depth;
    }
    if (corners_depth > last_depth) {
      return corners_depth;
    }
    return last_depth;
  }

  IDAstar ida;
  ida.is_solved = locdir_solved;
  ida.estimator = estimator;

  LocDirCube ldc;
  locdir_reset(&ldc);


  printf("Solving a few scrambles...\n");
  for (size_t j = 0; j < 5; j++) {
    locdir_scramble(&ldc);

    ida_star_solve(&ida, &ldc);

    printf("Found a solution in %zu moves:\n", ida.path_length - 1);

    for (size_t i = 0; i < ida.path_length; ++i) {
      Cube cube = to_cube(ida.path + i);
      render(&cube);
    }
  }

  printf("Collecting statistics...\n");

  clock_t start = clock();
  size_t total_solves = 10;
  size_t total_moves = 0;
  size_t min_moves = ~0ULL;
  size_t max_moves = 0;
  for (size_t i = 0; i < total_solves; ++i) {
    locdir_scramble(&ldc);
    ida_star_solve(&ida, &ldc);
    size_t num_moves = ida.path_length - 1;
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

  printf("Solved %zu scrambles in %g seconds (%g ms / solution).\n", total_solves, took, 1000 * took/total_solves);
  printf("Minimum number of moves in a solution = %zu\n", min_moves);
  printf("Average number of moves in a solution = %g\n", ((double)total_moves) / total_solves);
  printf("Maximum number of moves in a solution = %zu\n", max_moves);

  free_nibblebase(&first);
  free_nibblebase(&last);
  free_nibblebase(&corners);
}

int main() {
  srand(time(NULL));

  // solve_ll(1000000, 1000000, 1, true);
  // solve_f2l(1000000, 1000000, 1, 1000);
  // solve_cross();

  // solve_2x2x2();

  // solve_edges();

  solve_3x3x3();

  return EXIT_SUCCESS;
}
