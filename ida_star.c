typedef struct {
  LocDirCube path[SEQUENCE_MAX_LENGTH];
  size_t path_length;
  bool (*is_solved)(LocDirCube*);
  unsigned char (*estimator)(LocDirCube*);
  bool noisy;
} IDAstar;

const unsigned char FOUND = 254;

unsigned char ida_star_search(IDAstar *ida, unsigned char so_far, unsigned char bound) {
  LocDirCube *ldc = ida->path + (ida->path_length - 1);
  unsigned char lower_bound = so_far + (*ida->estimator)(ldc);
  if (lower_bound > bound) {
    return lower_bound;
  }
  if ((*ida->is_solved)(ldc)) {
    return FOUND;
  }
  unsigned char min = 255;
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

void ida_star_solve(IDAstar *ida, LocDirCube *ldc) {
  ida->path[0] = *ldc;
  ida->path_length = 1;

  unsigned char bound = (*ida->estimator)(ida->path);

  for (;;) {
    if (ida->noisy) {
      printf("IDA* bound = %d\n", bound);
    }
    unsigned char search_result = ida_star_search(ida, 0, bound);
    if (search_result == FOUND) {
      // Solution is stored in ida->path.
      return;
    }
    bound = search_result;
  }
}
