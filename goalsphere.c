typedef struct {
  HashSet *sets;
  size_t num_sets;
  size_t goal_hash;
  size_t (*hash_func)(LocDirCube*);
} GoalSphere;

/*
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

bool set_has(size_t *set, size_t size, size_t hash) {
  size_t halfway;
  size_t halfway_value;
  for (;;) {
    if (size == 0) {
      return false;
    }
    halfway = size >> 1;
    halfway_value = set[halfway];
    if (hash < halfway_value) {
      size = halfway;
      continue;
    }
    if (hash > halfway_value) {
      if (size <= halfway) {
        return false;
      }
      set += halfway + 1;
      size -= halfway + 1;
      continue;
    }
    return true;
  }
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
*/

unsigned char goalsphere_depth_(GoalSphere *sphere, size_t hash) {
  if (hash == sphere->goal_hash) {
    return 0;
  }
  for (unsigned char depth = 0; depth < sphere->num_sets; ++depth) {
    if (hashset_has(sphere->sets + depth, hash)) {
      return depth + 1;
    }
  }
  return UNKNOWN;
}

bool goalsphere_shell(GoalSphere *sphere, LocDirCube *ldc) {
  size_t last = sphere->num_sets - 1;
  return hashset_has(sphere->sets + last, (*sphere->hash_func)(ldc));
}

unsigned char goalsphere_depth(GoalSphere *sphere, LocDirCube *ldc, unsigned char search_depth) {
  LocDirCube path[SEQUENCE_MAX_LENGTH];
  path[0] = *ldc;
  size_t path_length = 1;

  unsigned char search(unsigned char so_far) {
    size_t hash = (*sphere->hash_func)(path + path_length - 1);
    unsigned char depth = goalsphere_depth_(sphere, hash);
    if (depth != UNKNOWN) {
      return depth;
    }
    if (so_far >= search_depth) {
      return UNKNOWN;
    }
    unsigned char min = UNKNOWN;
    for (size_t i = 0; i < NUM_STABLE_MOVES; ++i) {
      path[path_length] = path[path_length - 1];
      locdir_apply_stable(path + path_length, STABLE_MOVES[i]);
      bool in_path = false;
      for (size_t j = 0; j < path_length; ++j) {
        if (locdir_equals(path + j, path + path_length)) {
          in_path = true;
          break;
        }
      }
      if (in_path) {
        continue;
      }
      path_length++;
      unsigned char child_depth = search(so_far + 1);
      path_length--;
      if (child_depth < min) {
        min = child_depth;
      }
    }
    if (min == UNKNOWN) {
      return UNKNOWN;
    }
    return min + 1;
  }

  return search(0);
}

sequence goalsphere_solve(GoalSphere *sphere, LocDirCube *ldc, unsigned char search_depth, bool (*better)(sequence a, sequence b)) {
  if (sphere->num_sets < 1) {
    return INVALID;
  }
  LocDirCube aligned = *ldc;
  locdir_realign(&aligned);
  if (hashset_has(sphere->sets, sphere->hash_func(&aligned))) {
    return I;
  }
  LocDirCube path[SEQUENCE_MAX_LENGTH];
  path[0] = *ldc;
  size_t path_length = 1;

  sequence solve(unsigned char search_depth_) {
    unsigned char best_depth = UNKNOWN;
    LocDirCube children[NUM_MOVES - 1];
    bool best[NUM_MOVES - 1];
    size_t i = 0;
    for (enum move move = U; move <= MAX_MOVE; ++move) {
      children[i] = path[path_length - 1];
      locdir_apply(children + i, move);
      bool in_path = false;
      for (size_t j = 0; j < path_length; ++j) {
        if (locdir_equals(children + i, path + j)) {
          in_path = true;
          break;
        }
      }
      if (in_path) {
        best[i] = false;
        i++;
        continue;
      }
      aligned = children[i];
      locdir_realign(&aligned);
      unsigned char depth = goalsphere_depth(sphere, &aligned, search_depth_);
      if (depth < best_depth) {
        best_depth = depth;
        for (int idx = 0; idx < i; ++idx) {
          best[idx] = false;
        }
      }
      best[i] = (depth <= best_depth);
      i++;
    }

    sequence solution = INVALID;
    if (best_depth == 0) {
      i = 0;
      for (enum move move = U; move <= MAX_MOVE; ++move) {
        if (best[i] && (*better)(move, solution)) {
          solution = move;
        }
        i++;
      }
      return solution;
    }

    if (best_depth == UNKNOWN) {
      return INVALID;
    }

    if (search_depth_ > 0) {
      search_depth_--;
    }
    i = 0;
    for (enum move move = U; move <= MAX_MOVE; ++move) {
      if (best[i]) {
        path[path_length++] = children[i];
        sequence candidate = concat(move, solve(search_depth_));
        path_length--;
        if ((*better)(candidate, solution)) {
          solution = candidate;
        }
      }
      i++;
    }
    return solution;
  }

  return solve(search_depth);
}

const sequence SENTINEL = INVALID - 1;

sequence* goalsphere_solve_all(GoalSphere *sphere, LocDirCube *ldc, unsigned char search_depth) {
  if (sphere->num_sets < 1) {
    return NULL;
  }
  LocDirCube aligned = *ldc;
  locdir_realign(&aligned);
  if (hashset_has(sphere->sets, sphere->hash_func(&aligned))) {
    sequence *result = malloc(2 * sizeof(sequence));
    result[0] = I;
    result[1] = SENTINEL;
    return result;
  }
  LocDirCube path[SEQUENCE_MAX_LENGTH];
  path[0] = *ldc;
  size_t path_length = 1;

  sequence* solve(unsigned char search_depth_) {
    unsigned char best_depth = UNKNOWN;
    LocDirCube children[NUM_MOVES - 1];
    bool best[NUM_MOVES - 1];
    size_t i = 0;
    for (enum move move = U; move <= MAX_MOVE; ++move) {
      children[i] = path[path_length - 1];
      locdir_apply(children + i, move);
      bool in_path = false;
      for (size_t j = 0; j < path_length; ++j) {
        if (locdir_equals(children + i, path + j)) {
          in_path = true;
          break;
        }
      }
      if (in_path) {
        best[i] = false;
        i++;
        continue;
      }
      aligned = children[i];
      locdir_realign(&aligned);
      unsigned char depth = goalsphere_depth(sphere, &aligned, search_depth_);
      if (depth < best_depth) {
        best_depth = depth;
        for (int idx = 0; idx < i; ++idx) {
          best[idx] = false;
        }
      }
      best[i] = (depth <= best_depth);
      i++;
    }

    if (best_depth == 0) {
      size_t num_solutions = 0;
      for (i = 0; i < NUM_MOVES - 1; ++i) {
        if (best[i]) {
          num_solutions++;
        }
      }
      sequence *result = malloc((num_solutions + 1) * sizeof(sequence));
      num_solutions = 0;
      i = 0;
      for (enum move move = U; move <= MAX_MOVE; ++move) {
        if (best[i]) {
          result[num_solutions++] = move;
        }
        i++;
      }
      result[num_solutions] = SENTINEL;
      return result;
    }

    if (best_depth == UNKNOWN) {
      return NULL;
    }

    if (search_depth_ > 0) {
      search_depth_--;
    }
    size_t num_best = 0;
    for (i = 0; i < NUM_MOVES - 1; ++i) {
      if (best[i]) {
        num_best++;
      }
    }
    sequence **child_results = malloc(num_best * sizeof(sequence*));
    num_best = 0;
    size_t num_solutions = 0;
    i = 0;
    for (enum move move = U; move <= MAX_MOVE; ++move) {
      if (best[i]) {
        path[path_length++] = children[i];
        child_results[num_best] = solve(search_depth_);

        // Technically the NULL case shouldn't happen. (best_depth == UNKNOWN) should have triggered above,
        // but maybe this is related to hash collisions.
        if (child_results[num_best] != NULL) {
          size_t j = 0;
          for (;;) {
            sequence solution = child_results[num_best][j];
            if (solution == SENTINEL) {
              break;
            }
            child_results[num_best][j++] = concat(move, solution);
            num_solutions++;
          }
        }
        num_best++;
        path_length--;
      }
      i++;
    }
    if (num_solutions == 0) {
      free(child_results);
      return NULL;
    }
    sequence *result = malloc((num_solutions + 1) * sizeof(sequence));
    num_solutions = 0;
    for (i = 0; i < num_best; ++i) {
      if (child_results[i] == NULL) {
        continue;
      }
      size_t j = 0;
      for (;;) {
        sequence solution = child_results[i][j];
        if (solution == SENTINEL) {
          break;
        }
        result[num_solutions++] = child_results[i][j++];
      }
      free(child_results[i]);
    }
    result[num_solutions] = SENTINEL;
    free(child_results);
    return result;
  }

  return solve(search_depth);
}


void update_goalsphere(GoalSphere *sphere, LocDirCube *ldc, size_t depth, size_t max_depth) {
  size_t hash = (*sphere->hash_func)(ldc);
  if (depth >= max_depth) {
    if (goalsphere_depth_(sphere, hash) == UNKNOWN) {
      hashset_add(sphere->sets + sphere->num_sets, hash);
    }
    return;
  }
  for (size_t i = 0; i < NUM_STABLE_MOVES; ++i) {
    LocDirCube child = *ldc;
    locdir_apply_stable(&child, STABLE_MOVES[i]);
    update_goalsphere(sphere, &child, depth + 1, max_depth);
  }
}

GoalSphere init_goalsphere(LocDirCube *goal, size_t max_depth, size_t (*hash_func)(LocDirCube*)) {
  GoalSphere sphere;
  sphere.sets = malloc(max_depth * sizeof(HashSet));
  sphere.num_sets = 0;
  sphere.hash_func = hash_func;

  sphere.goal_hash = hash_func(goal);

  while (sphere.num_sets < max_depth) {
    int magnitude = 5;
    if (sphere.num_sets > 0) {
      magnitude = sphere.sets[sphere.num_sets - 1].magnitude + 3;
    }
    sphere.sets[sphere.num_sets] = init_hashset(magnitude, sphere.goal_hash);
    update_goalsphere(&sphere, goal, 0, sphere.num_sets + 1);
    sphere.num_sets++;
  }

  return sphere;
}

void free_goalsphere(GoalSphere *sphere) {
  for (size_t i = 0; i < sphere->num_sets; ++i) {
    free_hashset(sphere->sets + i);
  }
  free(sphere->sets);
}
