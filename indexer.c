typedef struct {
  Cube *cubes;
  Node *nodes;
  Node *root;
  size_t breakpoints[SEQUENCE_MAX_LENGTH];
  size_t size;
  size_t max_size;
} Indexer;

Indexer init_indexer(size_t max_size) {
  Indexer result;
  result.max_size = max_size;
  result.size = 0;
  result.cubes = malloc(max_size * sizeof(Cube));
  result.nodes = calloc(max_size, sizeof(Node));
  result.root = NULL;
  return result;
}

void free_indexer(Indexer *indexer) {
  free(indexer->cubes);
  free(indexer->nodes);
}

bool update_indexer(Indexer *indexer, Cube *cube) {
  if (get(indexer->root, cube) != NULL) {
    return false;
  }
  if (indexer->size >= indexer->max_size) {
    return false;
  }
  indexer->cubes[indexer->size].a = cube->a;
  indexer->cubes[indexer->size].b = cube->b;
  indexer->cubes[indexer->size].c = cube->c;
  indexer->nodes[indexer->size].cube = indexer->cubes + indexer->size;
  if (indexer->size) {
    insert(indexer->root, indexer->nodes + indexer->size);
  } else {
    indexer->root = indexer->nodes;
    indexer->root->cube = indexer->cubes;
  }
  indexer->size++;
  return true;
}

void update_indexer_variants(Indexer *indexer, Cube *cube) {
  for (enum move m = U; m <= MAX_MOVE; ++m) {
    Cube variant = *cube;
    apply(&variant, m);
    compensate(&variant, m);
    update_indexer(indexer, &variant);
  }
}

void fill_indexer(Indexer *indexer, Cube *cube) {
  size_t low = -1;
  size_t high = 0;
  int break_index = 0;
  update_indexer(indexer, cube);
  indexer->breakpoints[break_index++] = indexer->size;
  while (low != high) {
    low = high;
    high = indexer->size;
    for (size_t i = low; i < high; ++i) {
      update_indexer_variants(indexer, indexer->cubes + i);
    }
    indexer->root = balance(indexer->nodes, indexer->size);
    indexer->breakpoints[break_index++] = indexer->size;
  }
  indexer->max_size = indexer->size;
  indexer->cubes = realloc(indexer->cubes, indexer->size * sizeof(Cube));
  indexer->nodes = realloc(indexer->nodes, indexer->size * sizeof(Node));
  indexer->root = balance(indexer->nodes, indexer->size);
}

/* Move compensation means that depth is independent of orientation. */
Cube *indexed_get_(Node* root, Cube *cube) {
  // Yellow on top, green in front
  Cube *result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }

  // Orange on top, white in front
  rotate_x(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }

  // Blue on top, red in front
  rotate_x(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }

  // White on top, blue in front
  rotate_x_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }

  // Red on top, white in front
  rotate_x_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }

  // Green on top, orange in front
  rotate_x(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  result = get(root, cube);
  if (result) {
    return result;
  }
  rotate_y_prime(cube);
  return get(root, cube);
}

int indexed_depth(Indexer *indexer, Cube *cube) {
  Cube roller = *cube;
  Cube *existing = indexed_get_(indexer->root, &roller);
  if (existing == NULL) {
    fprintf(stderr, "Failed to index.\n");
    exit(EXIT_FAILURE);
  }
  size_t index = existing - indexer->cubes;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    if (index < indexer->breakpoints[i]) {
      return i;
    }
  }
}

sequence indexed_solution_(Indexer *indexer, Cube *cube) {
  int best_depth = SEQUENCE_MAX_LENGTH;
  Cube children[NUM_MOVES - 1];
  bool best[NUM_MOVES - 1];
  int child_idx = 0;
  for (enum move m = U; m <= MAX_MOVE; ++m) {
    children[child_idx] = *cube;
    apply(children + child_idx, m);
    int depth = indexed_depth(indexer, children + child_idx);
    if (depth < best_depth) {
      best_depth = depth;
      for (int idx = 0; idx < child_idx; ++idx) {
        best[idx] = false;
      }
    }
    best[child_idx] = (depth <= best_depth);
    child_idx++;
  }

  if (best_depth == 0) {
    child_idx = 0;
    for (enum move m = U; m <= MAX_MOVE; ++m) {
      if (best[child_idx++]) {
        return m;
      }
    }
  }

  sequence solution = INVALID;
  child_idx = 0;
  for (enum move m = U; m <= MAX_MOVE; ++m) {
    if (best[child_idx]) {
      sequence candidate = concat(m, indexed_solution_(indexer, children + child_idx));
      if (is_better(candidate, solution)) {
        solution = candidate;
      }
    }
    child_idx++;
  }
  return solution;
}

sequence indexed_solution(Indexer *indexer, Cube *cube) {
  if (equals(cube, indexer->cubes)) {
    return I;
  }
  return indexed_solution_(indexer, cube);
}
