typedef unsigned long long int bitboard;

typedef struct {
  bitboard a;
  bitboard b;
  bitboard c;
} Cube;

bool equals(Cube *a, Cube *b) {
  return a->a == b->a && a->b == b->b && a->c == b->c;
}

int compare(Cube *a, Cube *b) {
  if (a->c < b->c) {
    return -1;
  }
  if (a->c > b->c) {
    return 1;
  }
  if (a->b < b->b) {
    return -1;
  }
  if (a->b > b->b) {
    return 1;
  }
  if (a->a < b->a) {
    return -1;
  }
  if (a->a > b->a) {
    return 1;
  }
  return 0;
}

/* Reset cube to default solved configuration. */
void reset(Cube *cube) {
  cube->a = 0;
  cube->b = 0;
  cube->c = 0;
  for (int j = 0; j < 6; ++j) {
    for (int i = 0; i < 9; ++i) {
      bitboard p = 1ULL << (i + j*9);
      if ((j+1) & 1) {
        cube->a |= p;
      }
      if ((j+1) & 2) {
        cube->b |= p;
      }
      if ((j+1) & 4) {
        cube->c |= p;
      }
    }
  }
}

/* Reset cube to "solved" OLL configuration with side stickers removed from yellow pieces. */
void reset_oll(Cube *cube) {
  reset(cube);
  for (int j = 0; j < 4; ++j) {
    bitboard mask = ~(7ULL << (9*j));
    cube->a &= mask;
    cube->b &= mask;
    cube->c &= mask;
  }
}

const bitboard CROSS = 186ULL << (5*9);
const bitboard SIDE = 144ULL;

/* Reset cube to a "solved" bottom cross configuration with most stickers removed. */
void reset_cross(Cube *cube) {
  cube->a = 0;
  cube->b = CROSS;
  cube->c = CROSS;

  cube->a |= SIDE | (SIDE << 18);
  cube->b |= SIDE << 9 | (SIDE << 18);
  cube->c |= SIDE << 27;
}

/* Create a cube suitable for unit testing. */
Cube test_cube() {
  Cube cube;
  cube.a = 0;
  cube.b = 0;
  cube.c = 0;
  for (int i = 0; i < 6*9; ++i) {
    bitboard p = 1ULL << i;
    if (i % 2 || i == 0 || i == 30) {
      cube.a |= p;
    }
    if (i % 3) {
      cube.b |= p;
    }
    if (i % 5) {
      cube.c |= p;
    }
  }
  return cube;
}

/* Returns true if only the top layer is scrambled. */
bool is_top_layer(Cube *cube) {
  int q;
  bitboard a, b, c;
  for (int k = 0; k < 4; ++k) {
    q = 9*k + 3;
    a = (cube->a >> q) & 1;
    b = (cube->b >> q) & 1;
    c = (cube->c >> q) & 1;
    for (int i = 4; i < 9; ++i) {
      q = 9*k + i;
      if (((cube->a >> q) & 1) != a) {
        return false;
      }
      if (((cube->b >> q) & 1) != b) {
        return false;
      }
      if (((cube->c >> q) & 1) != c) {
        return false;
      }
    }
  }
  q = 9*5;
  a = (cube->a >> q) & 1;
  b = (cube->b >> q) & 1;
  c = (cube->c >> q) & 1;
  for (int i = 1; i < 9; ++i) {
    q = 9*5 + i;
    if (((cube->a >> q) & 1) != a) {
      return false;
    }
    if (((cube->b >> q) & 1) != b) {
      return false;
    }
    if (((cube->c >> q) & 1) != c) {
      return false;
    }
  }
  return true;
}

/* Returns true if only the top sides are scrambled. */
bool is_top_permutation(Cube *cube) {
  int q = 9*4;
  bitboard a = (cube->a >> q) & 1;
  bitboard b = (cube->b >> q) & 1;
  bitboard c = (cube->c >> q) & 1;
  for (int i = 1; i < 9; ++i) {
    q = 9*4 + i;
    if (((cube->a >> q) & 1) != a) {
      return false;
    }
    if (((cube->b >> q) & 1) != b) {
      return false;
    }
    if (((cube->c >> q) & 1) != c) {
      return false;
    }
  }
  return is_top_layer(cube);
}

/* Returns true if only the top layer is scrambled and the rest is in default position. */
bool is_yellow_layer(Cube *cube) {
  int q;
  bitboard a, b, c;
  for (int k = 0; k < 4; ++k) {
    a = (k+1) & 1;
    b = ((k+1)>>1) & 1;
    c = ((k+1)>>2) & 1;
    for (int i = 3; i < 9; ++i) {
      q = 9*k + i;
      if (((cube->a >> q) & 1) != a) {
        return false;
      }
      if (((cube->b >> q) & 1) != b) {
        return false;
      }
      if (((cube->c >> q) & 1) != c) {
        return false;
      }
    }
  }
  a = 0;
  b = 1;
  c = 1;
  for (int i = 0; i < 9; ++i) {
    q = 9*5 + i;
    if (((cube->a >> q) & 1) != a) {
      return false;
    }
    if (((cube->b >> q) & 1) != b) {
      return false;
    }
    if (((cube->c >> q) & 1) != c) {
      return false;
    }
  }
  return true;
}

/* Returns true if only the top sides are scrambled and the rest is in default position. */
bool is_yellow_permutation(Cube *cube) {
  int q;
  bitboard a = 1;
  bitboard b = 0;
  bitboard c = 1;
  for (int i = 0; i < 9; ++i) {
    q = 9*4 + i;
    if (((cube->a >> q) & 1) != a) {
      return false;
    }
    if (((cube->b >> q) & 1) != b) {
      return false;
    }
    if (((cube->c >> q) & 1) != c) {
      return false;
    }
  }
  return is_yellow_layer(cube);
}

const char* color_code(Cube *cube, int index) {
  bitboard p = 1ULL << index;
  bitboard color = !!(cube->a & p) + 2 * !!(cube->b & p) + 4 * !!(cube->c & p);
  switch (color) {
    case 0:
      return "\33[38;5;0m";
    case 1:
      return "\33[38;5;1m";
    case 2:
      return "\33[38;5;2m";
    case 3:
      return "\33[38;5;214m";
    case 4:
      return "\33[38;5;4m";
    case 5:
      return "\33[38;5;11m";
    case 6:
      return "\33[38;5;15m";
    case 7:
      return "\33[38;5;13m";
  }
  return "\33[38;5;14m";
}

void render_raw(Cube *cube) {
  for (int j = 0; j < 6; ++j) {
    for (int i = 0; i < 9; ++i) {
      printf("%s#", color_code(cube, i + 9 * j));
    }
    printf("\n");
  }
  printf("\33[0m");
  for (int i = 6*9; i < 64; ++i) {
    bitboard p = 1ULL << i;
    bitboard color = !!(cube->a & p) + 2 * !!(cube->b & p) + 4 * !!(cube->c & p);
    printf("%llu", color);
  }
  printf("\n");
}

void render(Cube *cube) {
  for (int j = 0; j < 3; ++j) {
    printf("      |");
    for (int i = 0; i < 3; ++i) {
      printf("%s#", color_code(cube, i + j*3 + 4*9));
      if (i < 2) {
        printf(" ");
      }
    }
    printf("\33[0m|\n");
  }
  for (int j = 0; j < 3; ++j) {
    for (int k = 0; k < 4; ++k) {
      printf("\33[0m|");
      for (int i = 0; i < 3; ++i) {
        printf("%s#", color_code(cube, i + j*3 + k*9));
        if (i < 2) {
          printf(" ");
        }
      }
    }
    printf("\33[0m|\n");
  }
  for (int j = 0; j < 3; ++j) {
    printf("      |");
    for (int i = 0; i < 3; ++i) {
      printf("%s#", color_code(cube, i + j*3 + 5*9));
      if (i < 2) {
        printf(" ");
      }
    }
    printf("\33[0m|\n");
  }

  printf("\33[0m");
}
