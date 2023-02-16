// Locations and directions/orientations of cubies

// TODO: Implement centers

typedef struct {
  char corner_locs[8];
  char corner_dirs[8];

  char edge_locs[12];
  bool edge_dirs[12];

  char center_locs[6];
} LocDirCube;

void locdir_reset(LocDirCube *ldc) {
  for (char i = 0; i < 8; ++i) {
    ldc->corner_locs[i] = i;
    ldc->corner_dirs[i] = 0;
  }
  for (char i = 0; i < 12; ++i) {
    ldc->edge_locs[i] = i;
    ldc->edge_dirs[i] = true;
  }
  for (char i = 0; i < 6; ++i) {
    ldc->center_locs[i] = i;
  }
}

void locdir_reset_corners(LocDirCube *ldc) {
  for (char i = 0; i < 8; ++i) {
    ldc->corner_locs[i] = i;
    ldc->corner_dirs[i] = 0;
  }
  for (char i = 0; i < 12; ++i) {
    ldc->edge_locs[i] = -1;
    ldc->edge_dirs[i] = true;
  }
  for (char i = 0; i < 6; ++i) {
    ldc->center_locs[i] = -1;
  }
}

void locdir_reset_edges(LocDirCube *ldc) {
  for (char i = 0; i < 8; ++i) {
    ldc->corner_locs[i] = -1;
    ldc->corner_dirs[i] = 0;
  }
  for (char i = 0; i < 12; ++i) {
    ldc->edge_locs[i] = i;
    ldc->edge_dirs[i] = true;
  }
  for (char i = 0; i < 6; ++i) {
    ldc->center_locs[i] = -1;
  }
}

bool locdir_equals(LocDirCube *a, LocDirCube *b) {
  for (char i = 0; i < 8; ++i) {
    if (a->corner_locs[i] != b->corner_locs[i]) {
      return false;
    }
    if (a->corner_dirs[i] != b->corner_dirs[i]) {
      return false;
    }
  }
  for (char i = 0; i < 12; ++i) {
    if (a->edge_locs[i] != b->edge_locs[i]) {
      return false;
    }
    if (a->edge_dirs[i] != b->edge_dirs[i]) {
      return false;
    }
  }
  return true;
}

bool locdir_edges_solved(LocDirCube *ldc) {
  for (char i = 0; i < 12; ++i) {
    if (ldc->edge_locs[i] != i) {
      return false;
    }
    if (!ldc->edge_dirs[i]) {
      return false;
    }
  }
  return true;
}

bool locdir_solved(LocDirCube *ldc) {
  for (char i = 0; i < 8; ++i) {
    if (ldc->corner_locs[i] != i) {
      return false;
    }
    if (ldc->corner_dirs[i] != 0) {
      return false;
    }
  }
  return locdir_edges_solved(ldc);
}

size_t locdir_corner_index(LocDirCube *ldc) {
  size_t result = 0;
  for (int i = 0; i < 8; ++i) {
    char loc = ldc->corner_locs[i];
    for (int j = i - 1; j >= 0; --j) {
      if (ldc->corner_locs[j] < ldc->corner_locs[i]) {
        loc--;
      }
    }
    result = loc + result * (8 - i);
    // The orientation of the last corner can be determined given the rest
    if (i < 7) {
      result = ldc->corner_dirs[i] + 3 * result;
    }
  }
  return result;
}

const size_t LOCDIR_CORNER_INDEX_SPACE = 8*7*6*5*4*3*2*1 * 3*3*3*3 * 3*3*3*(1);

size_t locdir_four_corner_index(LocDirCube *ldc) {
  size_t result = 0;
  for (int i = 0; i < 4; ++i) {
    char loc = ldc->corner_locs[i];
    for (int j = i - 1; j >= 0; --j) {
      if (ldc->corner_locs[j] < ldc->corner_locs[i]) {
        loc--;
      }
    }
    result = loc + result * (8 - i);
    result = ldc->corner_dirs[i] + 3 * result;
  }
  return result;
}

const size_t LOCDIR_FOUR_CORNER_INDEX_SPACE = 8*7*6*5 * 3*3*3*3;

size_t locdir_edge_index(LocDirCube *ldc) {
  size_t result = 0;
  for (int i = 0; i < 12; ++i) {
    char loc = ldc->edge_locs[i];
    for (int j = i - 1; j >= 0; --j) {
      if (ldc->edge_locs[j] < ldc->edge_locs[i]) {
        loc--;
      }
    }
    result = loc + result * (12 - i);
    // The orientation of the last edge can be determined given the rest
    if (i < 11) {
      result = ldc->edge_dirs[i] + 2 * result;
    }
  }
  return result;
}

const size_t LOCDIR_EDGE_INDEX_SPACE = 12ULL*11*10*9*8*7*6*5*4*3*2*1 * 2*2*2*2 * 2*2*2*2 * 2*2*2*(1);

size_t locdir_first_7_edge_index(LocDirCube *ldc) {
  size_t result = 0;
  for (int i = 0; i < 7; ++i) {
    char loc = ldc->edge_locs[i];
    for (int j = i - 1; j >= 0; --j) {
      if (ldc->edge_locs[j] < ldc->edge_locs[i]) {
        loc--;
      }
    }
    result = loc + result * (12 - i);
    result = ldc->edge_dirs[i] + 2 * result;
  }
  return result;
}

size_t locdir_last_7_edge_index(LocDirCube *ldc) {
  size_t result = 0;
  for (int i = 0; i < 7; ++i) {
    char loc = ldc->edge_locs[11 - i];
    for (int j = i - 1; j >= 0; --j) {
      if (ldc->edge_locs[11 - j] < ldc->edge_locs[11 - i]) {
        loc--;
      }
    }
    result = loc + result * (12 - i);
    result = ldc->edge_dirs[11 - i] + 2 * result;
  }
  return result;
}

const size_t LOCDIR_FIRST_7_EDGE_INDEX_SPACE = 12*11*10*9*8*7*6 * 2*2*2*2*2*2*2;
const size_t LOCDIR_LAST_7_EDGE_INDEX_SPACE = LOCDIR_FIRST_7_EDGE_INDEX_SPACE;

size_t locdir_first_4_edge_index(LocDirCube *ldc) {
  size_t result = 0;
  for (int i = 0; i < 4; ++i) {
    char loc = ldc->edge_locs[i];
    for (int j = i - 1; j >= 0; --j) {
      if (ldc->edge_locs[j] < ldc->edge_locs[i]) {
        loc--;
      }
    }
    result = loc + result * (12 - i);
    result = ldc->edge_dirs[i] + 2 * result;
  }
  return result;
}

size_t locdir_middle_4_edge_index(LocDirCube *ldc) {
  size_t result = 0;
  for (int i = 0; i < 4; ++i) {
    char loc = ldc->edge_locs[4 + i];
    for (int j = i - 1; j >= 0; --j) {
      if (ldc->edge_locs[4 + j] < ldc->edge_locs[4 + i]) {
        loc--;
      }
    }
    result = loc + result * (12 - i);
    result = ldc->edge_dirs[4 + i] + 2 * result;
  }
  return result;
}

size_t locdir_last_4_edge_index(LocDirCube *ldc) {
  size_t result = 0;
  for (int i = 0; i < 4; ++i) {
    char loc = ldc->edge_locs[11 - i];
    for (int j = i - 1; j >= 0; --j) {
      if (ldc->edge_locs[11 - j] < ldc->edge_locs[11 - i]) {
        loc--;
      }
    }
    result = loc + result * (12 - i);
    result = ldc->edge_dirs[11 - i] + 2 * result;
  }
  return result;
}

const size_t LOCDIR_FIRST_4_EDGE_INDEX_SPACE = 12*11*10*9 * 2*2*2*2;
const size_t LOCDIR_MIDDLE_4_EDGE_INDEX_SPACE = LOCDIR_FIRST_4_EDGE_INDEX_SPACE;
const size_t LOCDIR_LAST_4_EDGE_INDEX_SPACE = LOCDIR_FIRST_4_EDGE_INDEX_SPACE;

/* Index for an intermediary stage in OLL solving. */
size_t locdir_oll_index(LocDirCube *ldc) {
  size_t result = 0;
  // Bottom corners
  for (int i = 0; i < 4; ++i) {
    char loc = ldc->corner_locs[4 + i];
    for (int j = i - 1; j >= 0; --j) {
      if (ldc->corner_locs[4 + j] < ldc->corner_locs[4 + i]) {
        loc--;
      }
    }
    result = loc + result * (8 - i);
    result = ldc->corner_dirs[4 + i] + 3 * result;
  }
  // F2L edges
  for (int i = 0; i < 8; ++i) {
    char loc = ldc->edge_locs[4 + i];
    for (int j = i - 1; j >= 0; --j) {
      if (ldc->edge_locs[4 + j] < ldc->edge_locs[4 + i]) {
        loc--;
      }
    }
    result = loc + result * (12 - i);
    result = ldc->edge_dirs[4 + i] + 2 * result;
  }

  char top_corners[4];
  for (int i = 0; i < 4; ++i) {
    char loc = ldc->corner_locs[i];
    for (int j = 4; j < 8; ++j) {
      if (ldc->corner_locs[j] < ldc->corner_locs[i]) {
        loc--;
      }
    }
    top_corners[loc] = ldc->corner_dirs[i];
  }
  // Last orientation is implicit
  for (int i = 0; i < 3; ++i) {
    result = top_corners[i] + 3 * result;
  }

  bool top_edges[4];
  for (int i = 0; i < 4; ++i) {
    char loc = ldc->edge_locs[i];
    for (int j = 4; j < 12; ++j) {
      if (ldc->edge_locs[j] < ldc->edge_locs[i]) {
        loc--;
      }
    }
    top_edges[loc] = ldc->edge_dirs[i];
  }
  // Last orientation is implicit
  for (int i = 0; i < 3; ++i) {
    result = top_edges[i] + 2 * result;
  }
  return result;
}

const size_t LOCDIR_OLL_INDEX_SPACE = (8ULL*7*6*5 * 3*3*3*3) * (12ULL*11*10*9*8*7*6*5 * 2*2*2*2*2*2*2*2) * (3*3*3*(1)) * (2*2*2*(1));

size_t locdir_centerless_hash(LocDirCube *ldc) {
  return locdir_corner_index(ldc) ^ (18804110 * locdir_edge_index(ldc));
}

bitboard corner_to_bitboard(char loc, char dir) {
  dir %= 3;

  switch (loc) {
    // Corners touching U
    case 0:
      switch (dir) {
        case 0:
          return 1ULL;
        case 1:
          return 1ULL << (3*9 + 2);
        case 2:
          return 1ULL << (4*9);
      }

    case 1:
      switch (dir) {
        case 0:
          return 1ULL << 9;
        case 1:
          return 1ULL << 2;
        case 2:
          return 1ULL << (4*9 + 6);
      }

    case 2:
      switch (dir) {
        case 0:
          return 1ULL << 18;
        case 1:
          return 1ULL << (9 + 2);
        case 2:
          return 1ULL << (4*9 + 8);
      }

    case 3:
      switch (dir) {
        case 0:
          return 1ULL << 27;
        case 1:
          return 1ULL << (18 + 2);
        case 2:
          return 1ULL << (4*9 + 2);
      }

    // Corners touching D
    case 4:
      switch (dir) {
        case 0:
          return 1ULL << 6;
        case 1:
          return 1ULL << (5*9 + 6);
        case 2:
          return 1ULL << (3*9 + 8);
      }

    case 5:
      switch (dir) {
        case 0:
          return 1ULL << (9 + 6);
        case 1:
          return 1ULL << (5*9);
        case 2:
          return 1ULL << 8;
      }

    case 6:
      switch (dir) {
        case 0:
          return 1ULL << (18 + 6);
        case 1:
          return 1ULL << (5*9 + 2);
        case 2:
          return 1ULL << (9 + 8);
      }

    case 7:
      switch (dir) {
        case 0:
          return 1ULL << (27 + 6);
        case 1:
          return 1ULL << (5*9 + 8);
        case 2:
          return 1ULL << (18 + 8);
      }
  }

  return 0ULL;
}

bitboard edge_to_bitboard(char loc, bool dir) {
  switch (loc) {
    case 0:
      return dir ? 2ULL : 1ULL << (4*9 + 3);
    case 1:
      return dir ? 1ULL << (9 + 1) : 1ULL << (4*9 + 7);
    case 2:
      return dir ? 1ULL << (18 + 1) : 1ULL << (4*9 + 5);
    case 3:
      return dir ? 1ULL << (27 + 1) : 1ULL << (4*9 + 1);

    case 4:
      return dir ? 1ULL << 3 : 1ULL << (27 + 5);
    case 5:
      return dir ? 1ULL << (9 + 3) : 1ULL << 5;
    case 6:
      return dir ? 1ULL << (18 + 3) : 1ULL << (9 + 5);
    case 7:
      return dir ? 1ULL << (27 + 3) : 1ULL << (18 + 5);

    case 8:
      return dir ? 1ULL << 7 : 1ULL << (5*9 + 3);
    case 9:
      return dir ? 1ULL << (9 + 7) : 1ULL << (5*9 + 1);
    case 10:
      return dir ? 1ULL << (18 + 7) : 1ULL << (5*9 + 5);
    case 11:
      return dir ? 1ULL << (27 + 7) : 1ULL << (5*9 + 7);
  }

  return 0ULL;
}

bitboard center_to_bitboard(char loc) {
  if (0 <= loc && loc < 6) {
    return 16ULL << (9 * loc);
  }
  return 0ULL;
}

Cube to_cube(LocDirCube *ldc) {
  bitboard red = 0;
  bitboard green = 0;
  bitboard orange = 0;
  bitboard blue = 0;
  bitboard yellow = 0;
  bitboard white = 0;

  red |= corner_to_bitboard(ldc->corner_locs[0], ldc->corner_dirs[0]);
  blue |= corner_to_bitboard(ldc->corner_locs[0], ldc->corner_dirs[0] + 1);
  yellow |= corner_to_bitboard(ldc->corner_locs[0], ldc->corner_dirs[0] + 2);

  green |= corner_to_bitboard(ldc->corner_locs[1], ldc->corner_dirs[1]);
  red |= corner_to_bitboard(ldc->corner_locs[1], ldc->corner_dirs[1] + 1);
  yellow |= corner_to_bitboard(ldc->corner_locs[1], ldc->corner_dirs[1] + 2);

  orange |= corner_to_bitboard(ldc->corner_locs[2], ldc->corner_dirs[2]);
  green |= corner_to_bitboard(ldc->corner_locs[2], ldc->corner_dirs[2] + 1);
  yellow |= corner_to_bitboard(ldc->corner_locs[2], ldc->corner_dirs[2] + 2);

  blue |= corner_to_bitboard(ldc->corner_locs[3], ldc->corner_dirs[3]);
  orange |= corner_to_bitboard(ldc->corner_locs[3], ldc->corner_dirs[3] + 1);
  yellow |= corner_to_bitboard(ldc->corner_locs[3], ldc->corner_dirs[3] + 2);

  red |= corner_to_bitboard(ldc->corner_locs[4], ldc->corner_dirs[4]);
  blue |= corner_to_bitboard(ldc->corner_locs[4], ldc->corner_dirs[4] + 2);
  white |= corner_to_bitboard(ldc->corner_locs[4], ldc->corner_dirs[4] + 1);

  green |= corner_to_bitboard(ldc->corner_locs[5], ldc->corner_dirs[5]);
  red |= corner_to_bitboard(ldc->corner_locs[5], ldc->corner_dirs[5] + 2);
  white |= corner_to_bitboard(ldc->corner_locs[5], ldc->corner_dirs[5] + 1);

  orange |= corner_to_bitboard(ldc->corner_locs[6], ldc->corner_dirs[6]);
  green |= corner_to_bitboard(ldc->corner_locs[6], ldc->corner_dirs[6] + 2);
  white |= corner_to_bitboard(ldc->corner_locs[6], ldc->corner_dirs[6] + 1);

  blue |= corner_to_bitboard(ldc->corner_locs[7], ldc->corner_dirs[7]);
  orange |= corner_to_bitboard(ldc->corner_locs[7], ldc->corner_dirs[7] + 2);
  white |= corner_to_bitboard(ldc->corner_locs[7], ldc->corner_dirs[7] + 1);


  red |= edge_to_bitboard(ldc->edge_locs[0], ldc->edge_dirs[0]);
  yellow |= edge_to_bitboard(ldc->edge_locs[0], !ldc->edge_dirs[0]);

  green |= edge_to_bitboard(ldc->edge_locs[1], ldc->edge_dirs[1]);
  yellow |= edge_to_bitboard(ldc->edge_locs[1], !ldc->edge_dirs[1]);

  orange |= edge_to_bitboard(ldc->edge_locs[2], ldc->edge_dirs[2]);
  yellow |= edge_to_bitboard(ldc->edge_locs[2], !ldc->edge_dirs[2]);

  blue |= edge_to_bitboard(ldc->edge_locs[3], ldc->edge_dirs[3]);
  yellow |= edge_to_bitboard(ldc->edge_locs[3], !ldc->edge_dirs[3]);

  red |= edge_to_bitboard(ldc->edge_locs[4], ldc->edge_dirs[4]);
  blue |= edge_to_bitboard(ldc->edge_locs[4], !ldc->edge_dirs[4]);

  green |= edge_to_bitboard(ldc->edge_locs[5], ldc->edge_dirs[5]);
  red |= edge_to_bitboard(ldc->edge_locs[5], !ldc->edge_dirs[5]);

  orange |= edge_to_bitboard(ldc->edge_locs[6], ldc->edge_dirs[6]);
  green |= edge_to_bitboard(ldc->edge_locs[6], !ldc->edge_dirs[6]);

  blue |= edge_to_bitboard(ldc->edge_locs[7], ldc->edge_dirs[7]);
  orange |= edge_to_bitboard(ldc->edge_locs[7], !ldc->edge_dirs[7]);

  red |= edge_to_bitboard(ldc->edge_locs[8], ldc->edge_dirs[8]);
  white |= edge_to_bitboard(ldc->edge_locs[8], !ldc->edge_dirs[8]);

  green |= edge_to_bitboard(ldc->edge_locs[9], ldc->edge_dirs[9]);
  white |= edge_to_bitboard(ldc->edge_locs[9], !ldc->edge_dirs[9]);

  orange |= edge_to_bitboard(ldc->edge_locs[10], ldc->edge_dirs[10]);
  white |= edge_to_bitboard(ldc->edge_locs[10], !ldc->edge_dirs[10]);

  blue |= edge_to_bitboard(ldc->edge_locs[11], ldc->edge_dirs[11]);
  white |= edge_to_bitboard(ldc->edge_locs[11], !ldc->edge_dirs[11]);

  red |= center_to_bitboard(ldc->center_locs[0]);
  green |= center_to_bitboard(ldc->center_locs[1]);
  orange |= center_to_bitboard(ldc->center_locs[2]);
  blue |= center_to_bitboard(ldc->center_locs[3]);
  yellow |= center_to_bitboard(ldc->center_locs[4]);
  white |= center_to_bitboard(ldc->center_locs[5]);

  return (Cube) {red | orange | yellow, green | orange | white, blue | yellow | white};
}

/* Elementary operations */

static inline char corner_loc_U(char loc) {
  switch (loc) {
    case 0:
      return 3;
    case 1:
      return 0;
    case 2:
      return 1;
    case 3:
      return 2;
  }
  return loc;
}

// edge_loc_U spells the same

void locdir_U(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    ldc->corner_locs[i] = corner_loc_U(ldc->corner_locs[i]);
  }
  for (size_t i = 0; i < 12; ++i) {
    ldc->edge_locs[i] = corner_loc_U(ldc->edge_locs[i]);
  }
}

static inline char corner_loc_D_prime(char loc) {
  switch (loc) {
    case 4:
      return 7;
    case 5:
      return 4;
    case 6:
      return 5;
    case 7:
      return 6;
  }
  return loc;
}

// edge_loc_E_prime spells the same

static inline char edge_loc_D_prime(char loc) {
  switch (loc) {
    case 8:
      return 11;
    case 9:
      return 8;
    case 10:
      return 9;
    case 11:
      return 10;
  }
  return loc;
}

void locdir_D_prime(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    ldc->corner_locs[i] = corner_loc_D_prime(ldc->corner_locs[i]);
  }
  for (size_t i = 0; i < 12; ++i) {
    ldc->edge_locs[i] = edge_loc_D_prime(ldc->edge_locs[i]);
  }
}

void locdir_y(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    char loc = ldc->corner_locs[i];
    if (loc < 4) {
      ldc->corner_locs[i] = corner_loc_U(loc);
    } else {
      ldc->corner_locs[i] = corner_loc_D_prime(loc);
    }
  }
  for (size_t i = 0; i < 12; ++i) {
    char loc = ldc->edge_locs[i];
    if (loc < 4) {
      ldc->edge_locs[i] = corner_loc_U(loc);
    } else if (loc < 8) {
      ldc->edge_locs[i] = corner_loc_D_prime(loc);
    } else {
      ldc->edge_locs[i] = edge_loc_D_prime(loc);
    }
  }
  for (size_t i = 0; i < 6; ++i) {
    char loc = ldc->center_locs[i];
    if (loc < 4) {
      ldc->center_locs[i] = (loc + 3) % 4;
    }
  }
}

static inline char corner_loc_R(char loc) {
  switch (loc) {
    case 2:
      return 3;
    case 3:
      return 7;
    case 7:
      return 6;
    case 6:
      return 2;
  }
  return loc;
}

static inline char corner_dir_R(char loc, char dir) {
  switch (loc) {
    case 3:
      return (dir + 1) % 3;
    case 6:
      return (dir + 1) % 3;
    case 7:
      return (dir + 1) % 3;
  }
  return dir;
}

static inline char edge_loc_R(char loc) {
  switch (loc) {
    case 2:
      return 7;
    case 7:
      return 10;
    case 6:
      return 2;
    case 10:
      return 6;
  }
  return loc;
}

static inline char edge_dir_R(char loc, char dir) {
  switch (loc) {
    case 7:
      return !dir;
    case 10:
      return !dir;
  }
  return dir;
}

void locdir_R(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    ldc->corner_locs[i] = corner_loc_R(ldc->corner_locs[i]);
    ldc->corner_dirs[i] = corner_dir_R(ldc->corner_locs[i], ldc->corner_dirs[i]);
  }
  for (size_t i = 0; i < 12; ++i) {
    ldc->edge_locs[i] = edge_loc_R(ldc->edge_locs[i]);
    ldc->edge_dirs[i] = edge_dir_R(ldc->edge_locs[i], ldc->edge_dirs[i]);
  }
}

static inline char corner_loc_L_prime(char loc) {
  switch (loc) {
    case 1:
      return 0;
    case 0:
      return 4;
    case 4:
      return 5;
    case 5:
      return 1;
  }
  return loc;
}

static inline char corner_dir_L_prime(char loc, char dir) {
  switch (loc) {
    case 0:
      return (dir + 2) % 3;
    case 1:
      return (dir + 2) % 3;
    case 5:
      return (dir + 2) % 3;
  }
  return dir;
}

static inline char edge_loc_L_prime(char loc) {
  switch (loc) {
    case 0:
      return 4;
    case 4:
      return 8;
    case 5:
      return 0;
    case 8:
      return 5;
  }
  return loc;
}

static inline char edge_dir_L_prime(char loc, char dir) {
  switch (loc) {
    case 0:
      return !dir;
    case 5:
      return !dir;
  }
  return dir;
}

void locdir_L_prime(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    ldc->corner_locs[i] = corner_loc_L_prime(ldc->corner_locs[i]);
    ldc->corner_dirs[i] = corner_dir_L_prime(ldc->corner_locs[i], ldc->corner_dirs[i]);
  }
  for (size_t i = 0; i < 12; ++i) {
    ldc->edge_locs[i] = edge_loc_L_prime(ldc->edge_locs[i]);
    ldc->edge_dirs[i] = edge_dir_L_prime(ldc->edge_locs[i], ldc->edge_dirs[i]);
  }
}

static inline char edge_loc_M_prime(char loc) {
  switch (loc) {
    case 1:
      return 3;
    case 3:
      return 11;
    case 11:
      return 9;
    case 9:
      return 1;
  }
  return loc;
}

static inline char edge_dir_M_prime(char loc, char dir) {
  switch (loc) {
    case 3:
      return !dir;
    case 11:
      return !dir;
    case 9:
      return !dir;
    case 1:
      return !dir;
  }
  return dir;
}

static inline char center_loc_M_prime(char loc) {
  switch (loc) {
    case 1:
      return 4;
    case 4:
      return 3;
    case 3:
      return 5;
    case 5:
      return 1;
  }
  return loc;
}

void locdir_M_prime(LocDirCube *ldc) {
  for (size_t i = 0; i < 12; ++i) {
    ldc->edge_locs[i] = edge_loc_M_prime(ldc->edge_locs[i]);
    ldc->edge_dirs[i] = edge_dir_M_prime(ldc->edge_locs[i], ldc->edge_dirs[i]);
  }
  for (size_t i = 0; i < 6; ++i) {
    ldc->center_locs[i] = center_loc_M_prime(ldc->center_locs[i]);
  }
}

void locdir_x(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    char loc = corner_loc_R(corner_loc_L_prime(ldc->corner_locs[i]));
    ldc->corner_locs[i] = loc;
    ldc->corner_dirs[i] = corner_dir_R(loc, corner_dir_L_prime(loc, ldc->corner_dirs[i]));
  }
  for (size_t i = 0; i < 12; ++i) {
    char loc = edge_loc_M_prime(edge_loc_R(edge_loc_L_prime(ldc->edge_locs[i])));
    ldc->edge_locs[i] = loc;
    ldc->edge_dirs[i] = edge_dir_M_prime(loc, edge_dir_R(loc, edge_dir_L_prime(loc, ldc->edge_dirs[i])));
  }
  for (size_t i = 0; i < 6; ++i) {
    ldc->center_locs[i] = center_loc_M_prime(ldc->center_locs[i]);
  }
}

/* Unoptimized basic operations */

void locdir_U_prime(LocDirCube *ldc) {
  locdir_U(ldc);
  locdir_U(ldc);
  locdir_U(ldc);
}
void locdir_U2(LocDirCube *ldc) {
  locdir_U(ldc);
  locdir_U(ldc);
}

void locdir_D(LocDirCube *ldc) {
  locdir_D_prime(ldc);
  locdir_D_prime(ldc);
  locdir_D_prime(ldc);
}
void locdir_D2(LocDirCube *ldc) {
  locdir_D_prime(ldc);
  locdir_D_prime(ldc);
}

void locdir_R_prime(LocDirCube *ldc) {
  locdir_R(ldc);
  locdir_R(ldc);
  locdir_R(ldc);
}
void locdir_R2(LocDirCube *ldc) {
  locdir_R(ldc);
  locdir_R(ldc);
}
void locdir_L(LocDirCube *ldc) {
  locdir_L_prime(ldc);
  locdir_L_prime(ldc);
  locdir_L_prime(ldc);
}
void locdir_L2(LocDirCube *ldc) {
  locdir_L_prime(ldc);
  locdir_L_prime(ldc);
}

void locdir_y_prime(LocDirCube *ldc) {
  locdir_y(ldc);
  locdir_y(ldc);
  locdir_y(ldc);
}
void locdir_y2(LocDirCube *ldc) {
  locdir_y(ldc);
  locdir_y(ldc);
}

void locdir_x_prime(LocDirCube *ldc) {
  locdir_x(ldc);
  locdir_x(ldc);
  locdir_x(ldc);
}
void locdir_x2(LocDirCube *ldc) {
  locdir_x(ldc);
  locdir_x(ldc);
}

void locdir_z(LocDirCube *ldc) {
  locdir_y_prime(ldc);
  locdir_x(ldc);
  locdir_y(ldc);
}
void locdir_z_prime(LocDirCube *ldc) {
  locdir_y_prime(ldc);
  locdir_x_prime(ldc);
  locdir_y(ldc);
}
void locdir_z2(LocDirCube *ldc) {
  locdir_y_prime(ldc);
  locdir_x2(ldc);
  locdir_y(ldc);
}

void locdir_F(LocDirCube *ldc) {
  locdir_y_prime(ldc);
  locdir_R(ldc);
  locdir_y(ldc);
}
void locdir_F_prime(LocDirCube *ldc) {
  locdir_y(ldc);
  locdir_L_prime(ldc);
  locdir_y_prime(ldc);
}
void locdir_F2(LocDirCube *ldc) {
  locdir_y_prime(ldc);
  locdir_R2(ldc);
  locdir_y(ldc);
}

void locdir_B(LocDirCube *ldc) {
  locdir_y(ldc);
  locdir_R(ldc);
  locdir_y_prime(ldc);
}
void locdir_B_prime(LocDirCube *ldc) {
  locdir_y_prime(ldc);
  locdir_L_prime(ldc);
  locdir_y(ldc);
}
void locdir_B2(LocDirCube *ldc) {
  locdir_y(ldc);
  locdir_R2(ldc);
  locdir_y_prime(ldc);
}

void locdir_M(LocDirCube *ldc) {
  locdir_M_prime(ldc);
  locdir_M_prime(ldc);
  locdir_M_prime(ldc);
}
void locdir_M2(LocDirCube *ldc) {
  locdir_M_prime(ldc);
  locdir_M_prime(ldc);
}
void locdir_E(LocDirCube *ldc) {
  locdir_z(ldc);
  locdir_M(ldc);
  locdir_z_prime(ldc);
}
void locdir_E_prime(LocDirCube *ldc) {
  locdir_z(ldc);
  locdir_M_prime(ldc);
  locdir_z_prime(ldc);
}
void locdir_E2(LocDirCube *ldc) {
  locdir_z(ldc);
  locdir_M2(ldc);
  locdir_z_prime(ldc);
}
void locdir_S(LocDirCube *ldc) {
  locdir_y_prime(ldc);
  locdir_M_prime(ldc);
  locdir_y(ldc);
}
void locdir_S_prime(LocDirCube *ldc) {
  locdir_y_prime(ldc);
  locdir_M(ldc);
  locdir_y(ldc);
}
void locdir_S2(LocDirCube *ldc) {
  locdir_y_prime(ldc);
  locdir_M2(ldc);
  locdir_y(ldc);
}

#define NUM_STABLE_MOVES (27)

// Slices re-interpreted as synchronized opposite side turns
// Basically removes I, u, d, r, l, f and b.
enum move STABLE_MOVES[] = {
  U, U_prime,
  R, R_prime,
  F, F_prime,

  M,

  U2, R2, F2,
  M2,

  M_prime,

  D, D_prime, D2,

  L, L_prime, L2,
  B, B_prime, B2,

  E, E_prime, E2,
  S, S_prime, S2,
};

void locdir_apply_stable(LocDirCube *ldc, enum move move) {
  switch (move) {
    case U:
      locdir_U(ldc);
      break;
    case U_prime:
      locdir_U_prime(ldc);
      break;
    case U2:
      locdir_U2(ldc);
      break;
    case D:
      locdir_D(ldc);
      break;
    case D_prime:
      locdir_D_prime(ldc);
      break;
    case D2:
      locdir_D2(ldc);
      break;
    case R:
      locdir_R(ldc);
      break;
    case R_prime:
      locdir_R_prime(ldc);
      break;
    case R2:
      locdir_R2(ldc);
      break;
    case L:
      locdir_L(ldc);
      break;
    case L_prime:
      locdir_L_prime(ldc);
      break;
    case L2:
      locdir_L2(ldc);
      break;
    case F:
      locdir_F(ldc);
      break;
    case F_prime:
      locdir_F_prime(ldc);
      break;
    case F2:
      locdir_F2(ldc);
      break;
    case B:
      locdir_B(ldc);
      break;
    case B_prime:
      locdir_B_prime(ldc);
      break;
    case B2:
      locdir_B2(ldc);
      break;
    case M:
      locdir_R(ldc);
      locdir_L_prime(ldc);
      break;
    case M_prime:
      locdir_R_prime(ldc);
      locdir_L(ldc);
      break;
    case M2:
      locdir_R2(ldc);
      locdir_L2(ldc);
      break;
    case E:
      locdir_U(ldc);
      locdir_D_prime(ldc);
      break;
    case E_prime:
      locdir_U_prime(ldc);
      locdir_D(ldc);
      break;
    case E2:
      locdir_U2(ldc);
      locdir_D2(ldc);
      break;
    case S:
      locdir_F_prime(ldc);
      locdir_B(ldc);
      break;
    case S_prime:
      locdir_F(ldc);
      locdir_B_prime(ldc);
      break;
    case S2:
      locdir_F2(ldc);
      locdir_B2(ldc);
      break;
    default:
      fprintf(stderr, "Unimplemented stable move\n");
      exit(EXIT_FAILURE);
  }
}

void locdir_apply(LocDirCube *ldc, enum move move) {
  switch (move) {
    case I:
      break;
    case U:
      locdir_U(ldc);
      break;
    case U_prime:
      locdir_U_prime(ldc);
      break;
    case U2:
      locdir_U2(ldc);
      break;
    case D:
      locdir_D(ldc);
      break;
    case D_prime:
      locdir_D_prime(ldc);
      break;
    case D2:
      locdir_D2(ldc);
      break;
    case R:
      locdir_R(ldc);
      break;
    case R_prime:
      locdir_R_prime(ldc);
      break;
    case R2:
      locdir_R2(ldc);
      break;
    case L:
      locdir_L(ldc);
      break;
    case L_prime:
      locdir_L_prime(ldc);
      break;
    case L2:
      locdir_L2(ldc);
      break;
    case F:
      locdir_F(ldc);
      break;
    case F_prime:
      locdir_F_prime(ldc);
      break;
    case F2:
      locdir_F2(ldc);
      break;
    case B:
      locdir_B(ldc);
      break;
    case B_prime:
      locdir_B_prime(ldc);
      break;
    case B2:
      locdir_B2(ldc);
      break;
    case M:
      locdir_M(ldc);
      break;
    case M_prime:
      locdir_M_prime(ldc);
      break;
    case M2:
      locdir_M2(ldc);
      break;
    case E:
      locdir_E(ldc);
      break;
    case E_prime:
      locdir_E_prime(ldc);
      break;
    case E2:
      locdir_E2(ldc);
      break;
    case S:
      locdir_S(ldc);
      break;
    case S_prime:
      locdir_S_prime(ldc);
      break;
    case S2:
      locdir_S2(ldc);
      break;
    case u:
      locdir_y(ldc);
      locdir_D(ldc);
      break;
    case u_prime:
      locdir_y_prime(ldc);
      locdir_D_prime(ldc);
      break;
    case u2:
      locdir_y2(ldc);
      locdir_D2(ldc);
      break;
    case d:
      locdir_y_prime(ldc);
      locdir_U(ldc);
      break;
    case d_prime:
      locdir_y(ldc);
      locdir_U_prime(ldc);
      break;
    case d2:
      locdir_y2(ldc);
      locdir_U2(ldc);
      break;
    case r:
      locdir_x(ldc);
      locdir_L(ldc);
      break;
    case r_prime:
      locdir_x_prime(ldc);
      locdir_L_prime(ldc);
      break;
    case r2:
      locdir_x2(ldc);
      locdir_L2(ldc);
      break;
    case l:
      locdir_x_prime(ldc);
      locdir_R(ldc);
      break;
    case l_prime:
      locdir_x(ldc);
      locdir_R_prime(ldc);
      break;
    case l2:
      locdir_x2(ldc);
      locdir_R2(ldc);
      break;
    case f:
      locdir_z(ldc);
      locdir_B(ldc);
      break;
    case f_prime:
      locdir_z_prime(ldc);
      locdir_B_prime(ldc);
      break;
    case f2:
      locdir_z2(ldc);
      locdir_B2(ldc);
      break;
    case b:
      locdir_z_prime(ldc);
      locdir_F(ldc);
      break;
    case b_prime:
      locdir_z(ldc);
      locdir_F_prime(ldc);
      break;
    case b2:
      locdir_z2(ldc);
      locdir_F2(ldc);
      break;
    default:
      fprintf(stderr, "Unimplemented move\n");
      exit(EXIT_FAILURE);
  }
}

/* Rotate the cube so that the centers are in the standard position. */
void locdir_realign(LocDirCube *ldc) {
  // Move white to the bottom
  switch (ldc->center_locs[5]) {
    case 0:
      locdir_z_prime(ldc);
      break;
    case 1:
      locdir_x_prime(ldc);
      break;
    case 2:
      locdir_z(ldc);
      break;
    case 3:
      locdir_x(ldc);
      break;
    case 4:
      locdir_x2(ldc);
      break;
  }
  // Move green to the front
  switch (ldc->center_locs[1]) {
    case 0:
      locdir_y_prime(ldc);
      break;
    case 2:
      locdir_y(ldc);
      break;
    case 3:
      locdir_y2(ldc);
      break;
  }
}

void locdir_scramble(LocDirCube *ldc) {
  for (int i = 0; i < 100; ++i) {
    int r = rand() % 6;
    switch(r) {
      case 0:
        locdir_U(ldc);
        break;
      case 1:
        locdir_D(ldc);
        break;
      case 2:
        locdir_R(ldc);
        break;
      case 3:
        locdir_L(ldc);
        break;
      case 4:
        locdir_F(ldc);
        break;
      case 5:
        locdir_B(ldc);
        break;
    }
  }
}
