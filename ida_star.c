typedef struct {
  LocDirCube path[SEQUENCE_MAX_LENGTH];
  size_t path_length;
  bool (*is_solved)(LocDirCube*);
  unsigned char (*estimator)(LocDirCube*);
} IDAstar;

const unsigned char FOUND = 254;

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

void ida_star_solve(IDAstar *ida, LocDirCube *ldc, unsigned char lower_bound) {
  ida->path[0] = *ldc;
  ida->path_length = 1;

  unsigned char bound = (*ida->estimator)(ida->path);

  if (lower_bound > bound) {
    bound = lower_bound;
  }

  for (;;) {
    unsigned char search_result = ida_star_search(ida, 0, bound);
    if (search_result == FOUND) {
      // Solution is stored in ida->path.
      return;
    }
    bound = search_result;
  }
}

sequence ida_to_sequence(IDAstar *ida) {
  if (ida->path_length < 1) {
    return INVALID;
  }
  LocDirCube ldc = ida->path[0];

  sequence seq = I;

  for (size_t i = 0; i < ida->path_length; ++i) {
    for (enum move m = U; m < MAX_MOVE; ++m) {
      LocDirCube child = ldc;
      locdir_apply(&child, m);
      locdir_realign(&child);
      if (locdir_equals(&child, ida->path + i)) {
        seq = m + NUM_MOVES * seq;
        locdir_apply(&ldc, m);
        break;
      }
    }
  }

  return seq;
}
