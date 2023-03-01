#define LOG_IDA_STAR_PROGRESS 0

typedef struct {
  LocDirCube path[SEQUENCE_MAX_LENGTH];
  size_t path_length;
  bool (*is_solved)(LocDirCube*);
  unsigned char (*estimator)(LocDirCube*);
} IDAstar;

const unsigned char FOUND = 254;
const unsigned char SKIP = 253;

unsigned char ida_star_search(IDAstar *ida, unsigned char so_far, unsigned char bound) {
  LocDirCube *ldc = ida->path + (ida->path_length - 1);
  unsigned char to_go = (*ida->estimator)(ldc);
  unsigned char lower_bound = so_far + to_go;
  if (lower_bound > bound) {
    return lower_bound;
  }
  if (to_go == 0 && (*ida->is_solved)(ldc)) {
    return FOUND;
  }
  unsigned char min = UNKNOWN;
  for (size_t i = 0; i < NUM_STABLE_MOVES; ++i) {
    ida->path[ida->path_length] = *ldc;
    locdir_apply_stable(ida->path + ida->path_length, STABLE_MOVES[i]);

    bool in_path = false;
    for (size_t j = 0; j < ida->path_length; ++j) {
      if (locdir_equals(ida->path + ida->path_length, ida->path + j)) {
        in_path = true;
        break;
      }
    }
    if (in_path) {
      continue;
    }

    ida->path_length++;
    unsigned char child_result = ida_star_search(ida, so_far + 1, bound);
    if (child_result == FOUND) {
      return FOUND;
    }
    if (child_result < min) {
      min = child_result;
    }
    ida->path_length--;
  }
  return min;
}

void ida_star_solve(IDAstar *ida, LocDirCube *ldc, unsigned char lower_bound) {
  ida->path[0] = *ldc;
  ida->path_length = 1;

  unsigned char bound = (*ida->estimator)(ida->path);

  if (lower_bound > bound) {
    bound = lower_bound;
  }

  for (;;) {
    #if LOG_IDA_STAR_PROGRESS
    printf("IDA* bound = %d\n", bound);
    #endif
    unsigned char search_result = ida_star_search(ida, 0, bound);
    if (search_result == FOUND) {
      // Solution is stored in ida->path.
      return;
    }
    bound = search_result;
  }
}

void ida_star_solve_parallel(IDAstar *ida, LocDirCube *ldc, unsigned char lower_bound) {
  ida->path[0] = *ldc;
  ida->path_length = 1;
  // Check if already solved
  if ((*ida->is_solved)(ldc)) {
    return;
  }

  unsigned char bound = (*ida->estimator)(ida->path);

  if (lower_bound > bound) {
    bound = lower_bound;
  }

  IDAstar ida_team[NUM_STABLE_MOVES * NUM_STABLE_MOVES];
  for (int i = 0; i < NUM_STABLE_MOVES; ++i) {
    for (int j = 0; j < NUM_STABLE_MOVES; ++j) {
      int idx = i * NUM_STABLE_MOVES + j;
      ida_team[idx].path[0] = *ldc;
      ida_team[idx].path[1] = ida_team[idx].path[0];
      locdir_apply_stable(ida_team[idx].path + 1, STABLE_MOVES[i]);
      ida_team[idx].path[2] = ida_team[idx].path[1];
      locdir_apply_stable(ida_team[idx].path + 2, STABLE_MOVES[j]);
      ida_team[idx].path_length = 3;

      ida_team[idx].is_solved = ida->is_solved;
      ida_team[idx].estimator = ida->estimator;
    }
  }

  // Check if the first move already solves
  for (int i = 0; i < NUM_STABLE_MOVES * NUM_STABLE_MOVES; ++i) {
    if ((*ida->is_solved)(ida_team[i].path + 1)) {
      ida->path[0] = ida_team[i].path[0];
      ida->path[1] = ida_team[i].path[1];
      ida->path_length = 2;
      return;
    }
  }
  // Check if the second move already solves
  for (int i = 0; i < NUM_STABLE_MOVES * NUM_STABLE_MOVES; ++i) {
    if ((*ida->is_solved)(ida_team[i].path + 2)) {
      ida->path[0] = ida_team[i].path[0];
      ida->path[1] = ida_team[i].path[1];
      ida->path[2] = ida_team[i].path[2];
      ida->path_length = 3;
      return;
    }
  }

  unsigned char team_results[NUM_STABLE_MOVES * NUM_STABLE_MOVES] = {0};
  size_t num_members = 0;
  for (int i = 0; i < NUM_STABLE_MOVES * NUM_STABLE_MOVES; ++i) {
    // Skip backtracking
    if (
      locdir_equals(ida_team[i].path, ida_team[i].path + 1) ||
      locdir_equals(ida_team[i].path, ida_team[i].path + 2) ||
      locdir_equals(ida_team[i].path + 1, ida_team[i].path + 2)
    ) {
      // printf("%d = (%d, %d) backtracks\n", i, i % NUM_STABLE_MOVES, i / NUM_STABLE_MOVES);
      team_results[i] = SKIP;
      continue;
    }
    // Skip duplicates
    for (int j = 0; j < i; ++j) {
      if (team_results[j] == SKIP) {
        continue;
      }
      if (
        locdir_equals(ida_team[i].path + 2, ida_team[j].path + 1) ||
        locdir_equals(ida_team[i].path + 2, ida_team[j].path + 2)
      ) {
        // printf("%d = (%d, %d) is a duplicate\n", i, i % NUM_STABLE_MOVES, i / NUM_STABLE_MOVES);
        team_results[i] = SKIP;
        break;
      }
    }
    if (team_results[i] != SKIP) {
      num_members++;
    }
  }

  #if LOG_IDA_STAR_PROGRESS
  printf("%zu members in IDA* team\n", num_members);
  #endif


  for (;;) {
    #if LOG_IDA_STAR_PROGRESS
    printf("IDA* bound = %d\n", bound);
    #endif
    bool found = false;
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < NUM_STABLE_MOVES * NUM_STABLE_MOVES; ++i) {
      if (team_results[i] == SKIP || found) {
        continue;
      }
      team_results[i] = ida_star_search(ida_team + i, 2, bound);
      if (team_results[i] == FOUND) {
        found = true;
      }
    }

    bound = UNKNOWN;
    for (int i = 0; i < NUM_STABLE_MOVES * NUM_STABLE_MOVES; ++i) {
      if (team_results[i] == SKIP) {
        continue;
      }
      if (team_results[i] == FOUND) {
        // Store result in ida->path
        ida->path_length = ida_team[i].path_length;
        for (int j = 0; j < ida->path_length; ++j) {
          ida->path[j] = ida_team[i].path[j];
        }
        return;
      }
      bound = team_results[i] < bound ? team_results[i] : bound;
    }
  }
}

sequence ida_to_sequence(IDAstar *ida) {
  if (ida->path_length < 1) {
    return INVALID;
  }
  LocDirCube ldc = ida->path[0];

  sequence seq = I;

  for (size_t i = 0; i < ida->path_length; ++i) {
    for (enum move move = U; move < MAX_MOVE; ++move) {
      LocDirCube child = ldc;
      locdir_apply(&child, move);
      locdir_realign(&child);
      if (locdir_equals(&child, ida->path + i)) {
        seq = move + NUM_MOVES * seq;
        locdir_apply(&ldc, move);
        break;
      }
    }
  }

  return seq;
}
