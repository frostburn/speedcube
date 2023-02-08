#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"

typedef unsigned long long int bitboard;

typedef struct {
  bitboard a;
  bitboard b;
  bitboard c;
} Cube;

enum move {
  I,

  U, U_prime, U2,
  D, D_prime, D2,
  R, R_prime, R2,
  L, L_prime, L2,
  F, F_prime, F2,
  B, B_prime, B2,

  u, u_prime, u2,
  d, d_prime, d2,
  r, r_prime, r2,
  l, l_prime, l2,
  f, f_prime, f2,
  b, b_prime, b2,

  M, M_prime, M2,
  E, E_prime, E2,
  S, S_prime, S2
};

typedef unsigned __int128 sequence;

const sequence NUM_MOVES = 46;

/* Bitboard masks */
const bitboard U_BODY_MASK = 7 | (7 << 9) | (7 << 18);
const bitboard U_TAIL_MASK = 7 << 27;
const bitboard U_MASK = U_BODY_MASK | U_TAIL_MASK;
const bitboard U_FACE_0 = 1ULL << (4*9);
const bitboard U_FACE_1 = 1ULL << (4*9 + 1);
const bitboard U_FACE_2 = 1ULL << (4*9 + 2);
const bitboard U_FACE_3 = 1ULL << (4*9 + 3);
const bitboard U_FACE_5 = 1ULL << (4*9 + 5);
const bitboard U_FACE_6 = 1ULL << (4*9 + 6);
const bitboard U_FACE_7 = 1ULL << (4*9 + 7);
const bitboard U_FACE_8 = 1ULL << (4*9 + 8);
const bitboard U_FACE = U_FACE_0 | U_FACE_1 | U_FACE_2 | U_FACE_3 | U_FACE_5 | U_FACE_6 | U_FACE_7 | U_FACE_8;

const bitboard D_FACE_0 = 1ULL << (5*9);
const bitboard D_FACE_1 = 1ULL << (5*9 + 1);
const bitboard D_FACE_2 = 1ULL << (5*9 + 2);
const bitboard D_FACE_3 = 1ULL << (5*9 + 3);
const bitboard D_FACE_5 = 1ULL << (5*9 + 5);
const bitboard D_FACE_6 = 1ULL << (5*9 + 6);
const bitboard D_FACE_7 = 1ULL << (5*9 + 7);
const bitboard D_FACE_8 = 1ULL << (5*9 + 8);
const bitboard D_FACE = D_FACE_0 | D_FACE_1 | D_FACE_2 | D_FACE_3 | D_FACE_5 | D_FACE_6 | D_FACE_7 | D_FACE_8;

const bitboard B_FACE_0 = 1ULL << (3*9);
const bitboard B_FACE_1 = 1ULL << (3*9 + 1);
const bitboard B_FACE_2 = 1ULL << (3*9 + 2);
const bitboard B_FACE_3 = 1ULL << (3*9 + 3);
const bitboard B_FACE_5 = 1ULL << (3*9 + 5);
const bitboard B_FACE_6 = 1ULL << (3*9 + 6);
const bitboard B_FACE_7 = 1ULL << (3*9 + 7);
const bitboard B_FACE_8 = 1ULL << (3*9 + 8);
const bitboard B_FACE = B_FACE_0 | B_FACE_1 | B_FACE_2 | B_FACE_3 | B_FACE_5 | B_FACE_6 | B_FACE_7 | B_FACE_8;

const bitboard R_FACE_0 = 1ULL << (2*9);
const bitboard R_FACE_1 = 1ULL << (2*9 + 1);
const bitboard R_FACE_2 = 1ULL << (2*9 + 2);
const bitboard R_FACE_3 = 1ULL << (2*9 + 3);
const bitboard R_FACE_5 = 1ULL << (2*9 + 5);
const bitboard R_FACE_6 = 1ULL << (2*9 + 6);
const bitboard R_FACE_7 = 1ULL << (2*9 + 7);
const bitboard R_FACE_8 = 1ULL << (2*9 + 8);
const bitboard R_FACE = R_FACE_0 | R_FACE_1 | R_FACE_2 | R_FACE_3 | R_FACE_5 | R_FACE_6 | R_FACE_7 | R_FACE_8;

const bitboard L_FACE_0 = 1ULL;
const bitboard L_FACE_1 = 1ULL << 1;
const bitboard L_FACE_2 = 1ULL << 2;
const bitboard L_FACE_3 = 1ULL << 3;
const bitboard L_FACE_5 = 1ULL << 5;
const bitboard L_FACE_6 = 1ULL << 6;
const bitboard L_FACE_7 = 1ULL << 7;
const bitboard L_FACE_8 = 1ULL << 8;
const bitboard L_FACE = L_FACE_0 | L_FACE_1 | L_FACE_2 | L_FACE_3 | L_FACE_5 | L_FACE_6 | L_FACE_7 | L_FACE_8;

const bitboard Y_BODY_MASK = 511ULL | (511ULL << 9) | (511ULL << 18);
const bitboard Y_TAIL_MASK = 511ULL << 27;
const bitboard Y_MASK = Y_BODY_MASK | Y_TAIL_MASK;

const bitboard X_0 = 511ULL << (4*9);
const bitboard X_1 = 511ULL << (1*9);
const bitboard X_2 = 511ULL << (5*9);
const bitboard X_3 = 511ULL << (3*9);
const bitboard X_MASK = X_0 | X_1 | X_2 | X_3;

/* Face rotations */

inline bitboard U_face_prime(bitboard component) {
  return (
    ((component & U_FACE_0) << 6) |
    ((component & (U_FACE_1 | U_FACE_6)) << 2) |
    ((component & (U_FACE_2 | U_FACE_7)) >> 2) |
    ((component & U_FACE_3) << 4) |
    ((component & U_FACE_5) >> 4) |
    ((component & U_FACE_8) >> 6) |
    (component & ~U_FACE)
  );
}

inline bitboard D_face(bitboard component) {
  return (
    ((component & (D_FACE_0 | D_FACE_5)) << 2) |
    ((component & D_FACE_1) << 4) |
    ((component & D_FACE_2) << 6) |
    ((component & (D_FACE_3 | D_FACE_8)) >> 2) |
    ((component & D_FACE_6) >> 6) |
    ((component & D_FACE_7) >> 4) |
    (component & ~D_FACE)
  );
}

inline bitboard B_face(bitboard component) {
  return (
    ((component & (B_FACE_0 | B_FACE_5)) << 2) |
    ((component & B_FACE_1) << 4) |
    ((component & B_FACE_2) << 6) |
    ((component & (B_FACE_3 | B_FACE_8)) >> 2) |
    ((component & B_FACE_6) >> 6) |
    ((component & B_FACE_7) >> 4) |
    (component & ~B_FACE)
  );
}

inline bitboard R_face(bitboard component) {
  return (
    ((component & (R_FACE_0 | R_FACE_5)) << 2) |
    ((component & R_FACE_1) << 4) |
    ((component & R_FACE_2) << 6) |
    ((component & (R_FACE_3 | R_FACE_8)) >> 2) |
    ((component & R_FACE_6) >> 6) |
    ((component & R_FACE_7) >> 4) |
    (component & ~R_FACE)
  );
}

inline bitboard L_face_prime(bitboard component) {
  return (
    ((component & L_FACE_0) << 6) |
    ((component & (L_FACE_1 | L_FACE_6)) << 2) |
    ((component & (L_FACE_2 | L_FACE_7)) >> 2) |
    ((component & L_FACE_3) << 4) |
    ((component & L_FACE_5) >> 4) |
    ((component & L_FACE_8) >> 6) |
    (component & ~L_FACE)
  );
}

/* Elementary operations */

void turn_U_prime(Cube *cube) {
  // Sides
  cube->a = ((cube->a & U_BODY_MASK) << 9) | ((cube->a & U_TAIL_MASK) >> 27) | (cube->a & ~U_MASK);
  cube->b = ((cube->b & U_BODY_MASK) << 9) | ((cube->b & U_TAIL_MASK) >> 27) | (cube->b & ~U_MASK);
  cube->c = ((cube->c & U_BODY_MASK) << 9) | ((cube->c & U_TAIL_MASK) >> 27) | (cube->c & ~U_MASK);

  // Face
  cube->a = U_face_prime(cube->a);
  cube->b = U_face_prime(cube->b);
  cube->c = U_face_prime(cube->c);
}

// TODO: Elementary turn_R

void rotate_y_prime(Cube *cube) {
  // Permute side faces
  cube->a = ((cube->a & Y_BODY_MASK) << 9) | ((cube->a & Y_TAIL_MASK) >> 27) | (cube->a & ~Y_MASK);
  cube->b = ((cube->b & Y_BODY_MASK) << 9) | ((cube->b & Y_TAIL_MASK) >> 27) | (cube->b & ~Y_MASK);
  cube->c = ((cube->c & Y_BODY_MASK) << 9) | ((cube->c & Y_TAIL_MASK) >> 27) | (cube->c & ~Y_MASK);

  // Rotate top face
  cube->a = U_face_prime(cube->a);
  cube->b = U_face_prime(cube->b);
  cube->c = U_face_prime(cube->c);
  // Rotate bottom face
  cube->a = D_face(cube->a);
  cube->b = D_face(cube->b);
  cube->c = D_face(cube->c);
}

void rotate_x(Cube *cube) {
  // Re-orient faces for wrapping
  cube->a = U_face_prime(U_face_prime(cube->a));
  cube->b = U_face_prime(U_face_prime(cube->b));
  cube->c = U_face_prime(U_face_prime(cube->c));
  cube->a = B_face(B_face(cube->a));
  cube->b = B_face(B_face(cube->b));
  cube->c = B_face(B_face(cube->c));
  // Permute ring faces
  cube->a = ((cube->a & X_1) << (3*9)) | ((cube->a & X_2) >> (4*9)) | ((cube->a & X_3) << (2*9)) | ((cube->a & X_0) >> 9) | (cube->a & ~X_MASK);
  cube->b = ((cube->b & X_1) << (3*9)) | ((cube->b & X_2) >> (4*9)) | ((cube->b & X_3) << (2*9)) | ((cube->b & X_0) >> 9) | (cube->b & ~X_MASK);
  cube->c = ((cube->c & X_1) << (3*9)) | ((cube->c & X_2) >> (4*9)) | ((cube->c & X_3) << (2*9)) | ((cube->c & X_0) >> 9) | (cube->c & ~X_MASK);

  // Rotate right face
  cube->a = R_face(cube->a);
  cube->b = R_face(cube->b);
  cube->c = R_face(cube->c);
  // Rotate left face
  cube->a = L_face_prime(cube->a);
  cube->b = L_face_prime(cube->b);
  cube->c = L_face_prime(cube->c);
}

/* Unoptimized basic operations */

void turn_U(Cube *cube) {
  turn_U_prime(cube);
  turn_U_prime(cube);
  turn_U_prime(cube);
}

void turn_U2(Cube *cube) {
  turn_U_prime(cube);
  turn_U_prime(cube);
}

void rotate_y(Cube *cube) {
  rotate_y_prime(cube);
  rotate_y_prime(cube);
  rotate_y_prime(cube);
}

void rotate_x_prime(Cube *cube) {
  rotate_x(cube);
  rotate_x(cube);
  rotate_x(cube);
}

void rotate_z(Cube *cube) {
  rotate_y_prime(cube);
  rotate_x(cube);
  rotate_y(cube);
}

void rotate_z_prime(Cube *cube) {
  rotate_y_prime(cube);
  rotate_x_prime(cube);
  rotate_y(cube);
}

void turn_F(Cube *cube) {
  rotate_x(cube);
  turn_U(cube);
  rotate_x_prime(cube);
}

void turn_F_prime(Cube *cube) {
  rotate_x(cube);
  turn_U_prime(cube);
  rotate_x_prime(cube);
}

void turn_F2(Cube *cube) {
  rotate_x(cube);
  turn_U2(cube);
  rotate_x_prime(cube);
}

void turn_R(Cube *cube) {
  rotate_z_prime(cube);
  turn_U(cube);
  rotate_z(cube);
}

void turn_R_prime(Cube *cube) {
  rotate_z_prime(cube);
  turn_U_prime(cube);
  rotate_z(cube);
}
void turn_R2(Cube *cube) {
  rotate_z_prime(cube);
  turn_U2(cube);
  rotate_z(cube);
}


void apply(Cube *cube, enum move move) {
  switch (move) {
  case U:
    turn_U(cube);
    break;
  case U_prime:
    turn_U_prime(cube);
    break;
  case U2:
    turn_U2(cube);
    break;
  case R:
    turn_R(cube);
    break;
  case R_prime:
    turn_R_prime(cube);
    break;
  case R2:
    turn_R2(cube);
    break;
  case F:
    turn_F(cube);
    break;
  case F_prime:
    turn_F_prime(cube);
    break;
  case F2:
    turn_F2(cube);
    break;
  // TODO: Rest
  }
}

/* Utilities */

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

bool is_last_layer(Cube *cube) {
  if (is_top_layer(cube)) {
    return true;
  }

  Cube clone = *cube;
  rotate_x(&clone);
  if (is_top_layer(&clone)) {
    return true;
  }
  rotate_x(&clone);
  if (is_top_layer(&clone)) {
    return true;
  }
  rotate_x(&clone);
  if (is_top_layer(&clone)) {
    return true;
  }

  rotate_y_prime(&clone);
  rotate_x(&clone);
  if (is_top_layer(&clone)) {
    return true;
  }
  rotate_x(&clone);
  rotate_x(&clone);
  if (is_top_layer(&clone)) {
    return true;
  }
  return false;
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

void print_sequence(sequence seq) {
  sequence reversed = 0;
  for (int i = 0; i < 23; ++i) {
    reversed = reversed * NUM_MOVES + seq % NUM_MOVES;
    seq /= NUM_MOVES;
  }
  seq = reversed;
  for (int i = 0; i < 23; ++i) {
    switch (seq % NUM_MOVES) {
      case U:
        printf("U");
        break;
      case U_prime:
        printf("U'");
        break;
      case U2:
        printf("U2");
        break;
      case R:
        printf("R");
        break;
      case R_prime:
        printf("R'");
        break;
      case R2:
        printf("R2");
        break;
      case F:
        printf("F");
        break;
      case F_prime:
        printf("F'");
        break;
      case F2:
        printf("F2");
        break;
      // TODO: Rest
    }
    seq /= NUM_MOVES;
  }
  printf("\n");
}

bool equals(Cube *a, Cube *b) {
  return a->a == b->a && a->b == b->b && a->c == b->c;
}

void explore(Cube cube, sequence seq, int depth) {
  if (depth < 0) {
    return;
  }
  // TODO: Show non-trivial PLL as well
  if (is_top_layer(&cube) && (((cube.a & U_FACE) != U_FACE) || ((cube.b & U_FACE) != 0) || ((cube.c & U_FACE) != U_FACE))) {
    printf("Found something: ");
    print_sequence(seq);
    render(&cube);
  }
  for (int i = U; i <= U2; ++i) {
    Cube variant = cube;
    sequence var_seq = NUM_MOVES * seq + i;
    apply(&variant, i);
    explore(variant, var_seq, depth - 1);
  }

  for (int i = F; i <= F2; ++i) {
    Cube variant = cube;
    sequence var_seq = NUM_MOVES * seq + i;
    apply(&variant, i);
    explore(variant, var_seq, depth - 1);
  }

  for (int i = R; i <= R2; ++i) {
    Cube variant = cube;
    sequence var_seq = NUM_MOVES * seq + i;
    apply(&variant, i);
    explore(variant, var_seq, depth - 1);
  }
}

int main() {
  srand(time(NULL));
  Cube cube = {0};
  reset(&cube);

  explore(cube, 0, 7);

  return 0;
}
