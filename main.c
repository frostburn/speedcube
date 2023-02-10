#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"

#include "cube.c"
#include "moves.c"
#include "sequence.c"

typedef struct Node {
  Cube *cube;
  struct Node *left;
  struct Node *right;
} Node;

typedef struct {
  Cube *cubes;
  sequence *sequences;
  Node *nodes;
  Node *root;
  size_t size;
  size_t max_size;
} Database;


/* Utilities */

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

Cube* get(Node *node, Cube *cube) {
  if (node == NULL) {
    return NULL;
  }

  int cmp = compare(cube, node->cube);
  if (cmp == 0) {
    return node->cube;
  } else if (cmp < 0) {
    return get(node->left, cube);
  } else {
    return get(node->right, cube);
  }
}

void insert(Node *root, Node *leaf) {
  int cmp = compare(leaf->cube, root->cube);
  if (cmp < 0) {
    if (root->left == NULL) {
      root->left = leaf;
    } else {
      insert(root->left, leaf);
    }
    return;
  } else if (cmp > 0) {
    if (root->right == NULL) {
      root->right = leaf;
    } else {
      insert(root->right, leaf);
    }
    return;
  }

  fprintf(stderr, "Bad insert\n");
  exit(EXIT_FAILURE);
}

bool update(Database *database, Cube *cube, sequence seq) {
  Cube *existing = get(database->root, cube);
  if (existing != NULL) {
    size_t i = existing - database->cubes;
    if (seq < database->sequences[i]) {
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

size_t height(Node *root) {
  if (root == NULL) {
    return 0;
  }
  size_t left_height = height(root->left);
  size_t right_height = height(root->right);
  if (left_height > right_height) {
    return left_height + 1;
  }
  return right_height + 1;
}

int cmp_nodes(const void *a, const void *b) {
  Node *x = (Node*) a;
  Node *y = (Node*) b;
  return compare(x->cube, y->cube);
}

Node* rebuild(Node *nodes, size_t size) {
  if (!size) {
    return NULL;
  }
  size_t root_index = size / 2;
  size_t left_index = root_index / 2;
  size_t right_size = size - root_index - 1;
  size_t right_index = right_size / 2 + root_index + 1;

  Node *root = nodes + root_index;
  if (left_index == root_index) {
    root->left = NULL;
  } else {
    root->left = nodes + left_index;
    rebuild(nodes, root_index);
  }
  if (right_index == root_index || right_index >= size) {
    root->right = NULL;
  } else {
    root->right = nodes + right_index;
    rebuild(nodes + root_index + 1, right_size);
  }
  return root;
}

void balance(Database *database) {
  qsort(database->nodes, database->size, sizeof(Node), cmp_nodes);
  database->root = rebuild(database->nodes, database->size);
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
  size_t low = 0;
  size_t high = 0;
  update(database, cube, 0);
  while (database->size < database->max_size) {
    low = high;
    high = database->size;
    for (size_t i = low; i < high; ++i) {
      update_variants(database, database->cubes + i, database->sequences[i]);
    }
    balance(database);
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
    balance(database);
  }
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
    if (candidate < solution) {
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
        if (candidate < solution) {
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
    balance(&cache);
  }

  free_database(&cache);

  return solution;
}

void solve_pll(size_t database_size, size_t cache_size, size_t depth) {
  Cube cube = {0};
  reset(&cube);

  printf("Searching for yellow layer sequences...\n");
  Database database = init_database(database_size);

  fill_database(&database, &cube);

  sequence *sequences = malloc(1000 * sizeof(sequence));
  size_t num_sequences = 0;

  for (size_t i = 0; i < database.size; ++i) {
    if (is_yellow_layer(database.cubes + i)) {
      sequences[num_sequences] = database.sequences[i];
      num_sequences++;
    }
  }

  printf("Found %zu\n", num_sequences);

  printf("Using combinations to fill the whole yellow layer subgroup...\n");
  Database subgroup = init_database(100000);

  fill_subgroup(&subgroup, &cube, sequences, num_sequences);

  size_t num_permutations = 0;
  size_t num_solutions = 0;

  for (size_t i = 0; i < subgroup.size; ++i) {
    if (!is_yellow_layer(subgroup.cubes + i)) {
      fprintf(stderr, "Inconsistent element found\n");
      render(subgroup.cubes + i);
      exit(EXIT_FAILURE);
    }
    if (is_yellow_permutation(subgroup.cubes + i)) {
      sequence solution = solve(&database, subgroup.cubes + i, cache_size, depth);
      if (solution != INVALID) {
        num_solutions++;
      }
      print_sequence(solution);
      Cube front_view = subgroup.cubes[i];
      rotate_x_prime(&front_view);
      render(&front_view);
      num_permutations++;
    }
  }

  printf("Done with %zu elements of which %zu are permutations. Solved %zu of them.\n", subgroup.size, num_permutations, num_solutions);

  free_database(&database);
  free_database(&subgroup);
  free(sequences);
}

int main() {
  srand(time(NULL));

  solve_pll(30000000, 1000000, 1);

  return EXIT_SUCCESS;
}
