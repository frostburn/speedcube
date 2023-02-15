typedef struct {
  size_t **sets;
  size_t *set_sizes;
  size_t num_sets;
  size_t (*hash_func)(LocDirCube*);
} GoalSphere;

int cmp_size_t(const void *a, const void *b) {
  size_t x = *(size_t*)a;
  size_t y = *(size_t*)b;
  if (x < y) {
    return -1;
  }
  if (x > y) {
    return 1;
  }
  return 0;
}

const unsigned char UNKNOWN = 255;

bool set_has(size_t *set, size_t size, size_t hash) {
  if (size <= 0) {
    return false;
  }
  size_t halfway = size / 2;
  if (set[halfway] == hash) {
    return true;
  }
  if (hash < set[halfway]) {
    return set_has(set, halfway, hash);
  }
  if (size <= halfway) {
    return false;
  }
  return set_has(set + halfway + 1, size - halfway - 1, hash);
}

const size_t OUTSIDE = ~0ULL;

size_t set_index(size_t *set, size_t size, size_t hash) {
  if (size <= 0) {
    return OUTSIDE;
  }
  size_t halfway = size / 2;
  if (set[halfway] == hash) {
    return halfway;
  }
  if (hash < set[halfway]) {
    return set_index(set, halfway, hash);
  }
  if (size <= halfway) {
    return size + 1;
  }
  size_t right_index = set_index(set + halfway + 1, size - halfway - 1, hash);
  if (right_index == OUTSIDE) {
    return OUTSIDE;
  }
  return halfway + 1 + right_index;
}

unsigned char goalsphere_depth(GoalSphere *sphere, size_t hash) {
  for (unsigned char depth = 0; depth < sphere->num_sets; ++depth) {
    if (set_has(sphere->sets[depth], sphere->set_sizes[depth], hash)) {
      return depth;
    }
  }
  return UNKNOWN;
}

void update_goalsphere(GoalSphere *sphere, LocDirCube *ldc, size_t depth, size_t max_depth, bool *boundary) {
  size_t hash = (*sphere->hash_func)(ldc);
  if (depth == max_depth - 1) {
    size_t set_size = sphere->set_sizes[sphere->num_sets - 1];
    size_t boundary_index = set_index(sphere->sets[sphere->num_sets - 1], set_size, hash);
    if (boundary_index == OUTSIDE) {
      return;
    }
    if (boundary[boundary_index]) {
      return;
    }
    boundary[boundary_index] = true;
  } else if (depth >= max_depth) {
    sphere->sets[sphere->num_sets][sphere->set_sizes[sphere->num_sets]] = hash;
    sphere->set_sizes[sphere->num_sets]++;
    return;
  }
  for (size_t i = 0; i < NUM_STABLE_MOVES; ++i) {
    LocDirCube child = *ldc;
    locdir_apply_stable(&child, STABLE_MOVES[i]);
    update_goalsphere(sphere, &child, depth + 1, max_depth, boundary);
  }
}

GoalSphere init_goalsphere(LocDirCube *goal, size_t max_depth, size_t (*hash_func)(LocDirCube*)) {
  GoalSphere sphere;
  sphere.sets = malloc((max_depth + 1) * sizeof(size_t*));
  sphere.set_sizes = malloc((max_depth + 1) * sizeof(size_t));
  sphere.num_sets = 0;
  sphere.hash_func = hash_func;

  sphere.sets[0] = malloc(sizeof(size_t));
  sphere.sets[0][0] = hash_func(goal);
  sphere.set_sizes[0] = 1;
  sphere.num_sets++;

  for (size_t depth = 1; depth <= max_depth; ++depth) {
    bool *boundary = calloc(sphere.set_sizes[depth - 1], sizeof(bool));

    sphere.sets[depth] = malloc(sphere.set_sizes[depth - 1] * NUM_STABLE_MOVES * sizeof(size_t));
    sphere.set_sizes[depth] = 0;
    update_goalsphere(&sphere, goal, 0, depth, boundary);
    free(boundary);
    qsort(sphere.sets[depth], sphere.set_sizes[depth], sizeof(size_t), cmp_size_t);
    size_t num_unique = 0;
    for (size_t i = 0; i < sphere.set_sizes[depth]; ++i) {
      size_t hash = sphere.sets[depth][i];
      size_t previous = (i > 0) ? sphere.sets[depth][i-1] : ~hash;
      if (hash != previous && goalsphere_depth(&sphere, hash) == UNKNOWN) {
        sphere.sets[depth][num_unique++] = hash;
      }
    }
    sphere.sets[depth] = realloc(sphere.sets[depth], num_unique * sizeof(size_t));
    sphere.set_sizes[depth] = num_unique;
    sphere.num_sets++;
  }

  return sphere;
}

void free_goalsphere(GoalSphere *sphere) {
  for (size_t i = 0; i < sphere->num_sets; ++i) {
    free(sphere->sets[i]);
  }
  free(sphere->sets);
  free(sphere->set_sizes);
}
