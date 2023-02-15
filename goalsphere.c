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
  if (size - halfway <= 0) {
    return false;
  }
  return set_has(set + halfway + 1, size - halfway - 1, hash);
}

const size_t LAST_BIT = 1ULL << 63;

bool set_had(size_t *set, size_t size, size_t hash) {
  if (size <= 0) {
    return false;
  }
  size_t halfway = size / 2;
  if (hash == set[halfway]) {
    set[halfway] |= LAST_BIT;
    return true;
  }
  if ((hash & ~LAST_BIT) < (set[halfway] & ~LAST_BIT)) {
    return set_had(set, halfway, hash);
  }
  if (size - halfway <= 0) {
    return false;
  }
  return set_had(set + halfway + 1, size - halfway - 1, hash);
}

unsigned char goalsphere_depth(GoalSphere *sphere, size_t hash) {
  for (unsigned char depth = 0; depth < sphere->num_sets; ++depth) {
    if (set_has(sphere->sets[depth], sphere->set_sizes[depth], hash)) {
      return depth;
    }
  }
  return UNKNOWN;
}

void update_goalsphere(GoalSphere *sphere, LocDirCube *ldc, size_t depth, size_t max_depth, size_t *boundary) {
  size_t hash = (*sphere->hash_func)(ldc);
  if (goalsphere_depth(sphere, hash) < depth) {
    return;
  }
  if (depth == max_depth - 1) {
    if (!set_had(boundary, sphere->set_sizes[sphere->num_sets - 1], hash)) {
      return;
    }
  }
  if (depth >= max_depth) {
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
    size_t *boundary = malloc(sphere.set_sizes[depth - 1] * sizeof(size_t));
    for (size_t i = 0; i < sphere.set_sizes[depth -1]; ++i) {
      boundary[i] = sphere.sets[depth - 1][i] & ~LAST_BIT;
    }

    sphere.sets[depth] = malloc(sphere.set_sizes[depth - 1] * NUM_STABLE_MOVES * sizeof(size_t));
    sphere.set_sizes[depth] = 0;
    update_goalsphere(&sphere, goal, 0, depth, boundary);
    free(boundary);
    qsort(sphere.sets[depth], sphere.set_sizes[depth], sizeof(size_t), cmp_size_t);
    size_t num_unique = 1;
    for (size_t i = 1; i < sphere.set_sizes[depth]; ++i) {
      if (sphere.sets[depth][i - 1] != sphere.sets[depth][i]) {
        sphere.sets[depth][num_unique++] = sphere.sets[depth][i];
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
