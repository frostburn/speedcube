// Locations and directions/orientations of cubies

typedef struct {
  char corner_locs[8];
  char corner_dirs[8];

  char edge_locs[12];
  bool edge_dirs[12];

  char center_locs[6];
} LocDirCube;

void locdir_reset(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    ldc->corner_locs[i] = i;
    ldc->corner_dirs[i] = 0;
  }
  for (size_t i = 0; i < 12; ++i) {
    ldc->edge_locs[i] = i;
    ldc->edge_dirs[i] = false;
  }
  for (size_t i = 0; i < 6; ++i) {
    ldc->center_locs[i] = i;
  }
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

void locdir_U(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    ldc->corner_locs[i] = corner_loc_U(ldc->corner_locs[i]);
  }
}

void locdir_D_prime(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    ldc->corner_locs[i] = corner_loc_D_prime(ldc->corner_locs[i]);
  }
}

void locdir_y(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    if (ldc->corner_locs[i] < 4) {
      ldc->corner_locs[i] = corner_loc_U(ldc->corner_locs[i]);
    } else {
      ldc->corner_locs[i] = corner_loc_D_prime(ldc->corner_locs[i]);
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

void locdir_R(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    ldc->corner_locs[i] = corner_loc_R(ldc->corner_locs[i]);
    ldc->corner_dirs[i] = corner_dir_R(ldc->corner_locs[i], ldc->corner_dirs[i]);
  }
}

void locdir_L_prime(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    ldc->corner_locs[i] = corner_loc_L_prime(ldc->corner_locs[i]);
    ldc->corner_dirs[i] = corner_dir_L_prime(ldc->corner_locs[i], ldc->corner_dirs[i]);
  }
}

void locdir_x(LocDirCube *ldc) {
  for (size_t i = 0; i < 8; ++i) {
    char loc = corner_loc_R(corner_loc_L_prime(ldc->corner_locs[i]));
    ldc->corner_locs[i] = loc;
    ldc->corner_dirs[i] = corner_dir_R(loc, corner_dir_L_prime(loc, ldc->corner_dirs[i]));
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
