const unsigned char UNKNOWN = 255;

typedef struct {
  unsigned char *octets;
  unsigned char *visits;
  size_t num_visits;
  size_t (*index_func)(LocDirCube*);
} Nibblebase;

unsigned char get_nibble(Nibblebase *tablebase, size_t index) {
  if (index & 1) {
    return tablebase->octets[index / 2] >> 4;
  }
  return tablebase->octets[index / 2] & 0xF;
}

void set_nibble(Nibblebase *tablebase, size_t index, unsigned char value) {
  size_t idx = index / 2;
  if (index & 1) {
    tablebase->octets[idx] = (tablebase->octets[idx] & 0xF) | (value << 4);
  } else {
    tablebase->octets[idx] = value | (tablebase->octets[idx] & 0xF0);
  }
}

bool visit(Nibblebase *tablebase, size_t index) {
  size_t idx = index / 8;
  unsigned char p = 1 << (index & 7);
  if (tablebase->visits[idx] & p) {
    return true;
  }
  tablebase->visits[idx] |= p;
  return false;
}

Nibblebase init_nibblebase(size_t search_space_size, size_t (*index_func)(LocDirCube*)) {
  Nibblebase tablebase;
  size_t num_octets = (search_space_size + 1) / 2;
  tablebase.octets = malloc(num_octets * sizeof(unsigned char));
  tablebase.num_visits = (search_space_size + 7) / 8;
  tablebase.visits = calloc(tablebase.num_visits, sizeof(unsigned char));

  for (size_t i = 0; i < num_octets; ++i) {
    tablebase.octets[i] = 0xFF;
  }

  tablebase.index_func = index_func;

  return tablebase;
}

void free_nibblebase(Nibblebase *tablebase) {
  free(tablebase->octets);
  free(tablebase->visits);
}

bool update_nibblebase(Nibblebase *tablebase, LocDirCube *ldc, unsigned char depth, unsigned char max_depth) {
  if (depth >= max_depth) {
    return false;
  }

  size_t index = (*tablebase->index_func)(ldc);

  bool did_update = false;
  unsigned char tablebase_depth = get_nibble(tablebase, index);
  if (depth <= tablebase_depth) {
    bool occupied = visit(tablebase, index);

    if (depth < tablebase_depth) {
      set_nibble(tablebase, index, depth);
      did_update = true;
    } else if (occupied) {
      return false;
    }
    for (size_t i = 0; i < NUM_STABLE_MOVES; ++i) {
      LocDirCube child = *ldc;
      locdir_apply_stable(&child, STABLE_MOVES[i]);
      did_update = update_nibblebase(tablebase, &child, depth + 1, max_depth) || did_update;
    }
  }
  return did_update;
}

void populate_nibblebase(Nibblebase *tablebase, LocDirCube *ldc) {
  for (unsigned char max_depth = 1; max_depth < 16; ++max_depth) {
    bool did_update = update_nibblebase(tablebase, ldc, 0, max_depth);
    if (!did_update) {
      break;
    }
    for (size_t i = 0; i < tablebase->num_visits; ++i) {
      tablebase->visits[i] = 0;
    }
  }
}

unsigned char nibble_depth(Nibblebase *tablebase, LocDirCube *ldc) {
  return get_nibble(tablebase, (*tablebase->index_func)(ldc));
}

sequence nibble_solve(Nibblebase *tablebase, LocDirCube *ldc, bool (*better)(sequence a, sequence b)) {
  LocDirCube aligned = *ldc;
  locdir_realign(&aligned);
  unsigned char depth = nibble_depth(tablebase, &aligned);
  if (depth == 0) {
    return I;
  }

  sequence solve(LocDirCube *parent) {
    unsigned char best_depth = UNKNOWN;
    LocDirCube children[NUM_MOVES - 1];
    bool best[NUM_MOVES - 1];
    size_t i = 0;
    for (enum move m = U; m <= MAX_MOVE; ++m) {
      children[i] = *parent;
      locdir_apply(children + i, m);
      aligned = children[i];
      locdir_realign(&aligned);
      size_t index = (*tablebase->index_func)(&aligned);
      unsigned char depth = get_nibble(tablebase, index);
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
      for (enum move m = U; m <= MAX_MOVE; ++m) {
        if (best[i] && (*better)(m, solution)) {
          solution = m;
        }
        i++;
      }
      return solution;
    }

    if (best_depth == UNKNOWN) {
      return INVALID;
    }

    i = 0;
    for (enum move m = U; m <= MAX_MOVE; ++m) {
      if (best[i]) {
        sequence candidate = concat(m, solve(children + i));
        if ((*better)(candidate, solution)) {
          solution = candidate;
        }
      }
      i++;
    }
    return solution;
  }

  return solve(ldc);
}
