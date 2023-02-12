#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"

#include "cube.c"
#include "moves.c"
#include "sequence.c"
#include "bst.c"
#include "indexer.c"

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

int main() {
  srand(time(NULL));

  // solve_ll(1000000, 1000000, 1, true);
  // solve_f2l(1000000, 1000000, 1, 1000);
  solve_cross();

  return EXIT_SUCCESS;
}
