#include "stdio.h"

typedef unsigned long long int bitboard;

typedef struct {
  bitboard a;
  bitboard b;
  bitboard c;
} Cube;

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

static inline void U_face_prime(Cube *cube) {
  cube->a = (
    ((cube->a & U_FACE_0) << 6) |
    ((cube->a & (U_FACE_1 | U_FACE_6)) << 2) |
    ((cube->a & (U_FACE_2 | U_FACE_7)) >> 2) |
    ((cube->a & U_FACE_3) << 4) |
    ((cube->a & U_FACE_5) >> 4) |
    ((cube->a & U_FACE_8) >> 6) |
    (cube->a & ~U_FACE)
  );
  cube->b = (
    ((cube->b & U_FACE_0) << 6) |
    ((cube->b & (U_FACE_1 | U_FACE_6)) << 2) |
    ((cube->b & (U_FACE_2 | U_FACE_7)) >> 2) |
    ((cube->b & U_FACE_3) << 4) |
    ((cube->b & U_FACE_5) >> 4) |
    ((cube->b & U_FACE_8) >> 6) |
    (cube->b & ~U_FACE)
  );
  cube->c = (
    ((cube->c & U_FACE_0) << 6) |
    ((cube->c & (U_FACE_1 | U_FACE_6)) << 2) |
    ((cube->c & (U_FACE_2 | U_FACE_7)) >> 2) |
    ((cube->c & U_FACE_3) << 4) |
    ((cube->c & U_FACE_5) >> 4) |
    ((cube->c & U_FACE_8) >> 6) |
    (cube->c & ~U_FACE)
  );
}

static inline void D_face(Cube *cube) {
  cube->a = (
    ((cube->a & (D_FACE_0 | D_FACE_5)) << 2) |
    ((cube->a & D_FACE_1) << 4) |
    ((cube->a & D_FACE_2) << 6) |
    ((cube->a & (D_FACE_3 | D_FACE_8)) >> 2) |
    ((cube->a & D_FACE_6) >> 6) |
    ((cube->a & D_FACE_7) >> 4) |
    (cube->a & ~D_FACE)
  );
  cube->b = (
    ((cube->b & (D_FACE_0 | D_FACE_5)) << 2) |
    ((cube->b & D_FACE_1) << 4) |
    ((cube->b & D_FACE_2) << 6) |
    ((cube->b & (D_FACE_3 | D_FACE_8)) >> 2) |
    ((cube->b & D_FACE_6) >> 6) |
    ((cube->b & D_FACE_7) >> 4) |
    (cube->b & ~D_FACE)
  );
  cube->c = (
    ((cube->c & (D_FACE_0 | D_FACE_5)) << 2) |
    ((cube->c & D_FACE_1) << 4) |
    ((cube->c & D_FACE_2) << 6) |
    ((cube->c & (D_FACE_3 | D_FACE_8)) >> 2) |
    ((cube->c & D_FACE_6) >> 6) |
    ((cube->c & D_FACE_7) >> 4) |
    (cube->c & ~D_FACE)
  );
}

static inline void B_face(Cube *cube) {
  cube->a = (
    ((cube->a & (B_FACE_0 | B_FACE_5)) << 2) |
    ((cube->a & B_FACE_1) << 4) |
    ((cube->a & B_FACE_2) << 6) |
    ((cube->a & (B_FACE_3 | B_FACE_8)) >> 2) |
    ((cube->a & B_FACE_6) >> 6) |
    ((cube->a & B_FACE_7) >> 4) |
    (cube->a & ~B_FACE)
  );
  cube->b = (
    ((cube->b & (B_FACE_0 | B_FACE_5)) << 2) |
    ((cube->b & B_FACE_1) << 4) |
    ((cube->b & B_FACE_2) << 6) |
    ((cube->b & (B_FACE_3 | B_FACE_8)) >> 2) |
    ((cube->b & B_FACE_6) >> 6) |
    ((cube->b & B_FACE_7) >> 4) |
    (cube->b & ~B_FACE)
  );
  cube->c = (
    ((cube->c & (B_FACE_0 | B_FACE_5)) << 2) |
    ((cube->c & B_FACE_1) << 4) |
    ((cube->c & B_FACE_2) << 6) |
    ((cube->c & (B_FACE_3 | B_FACE_8)) >> 2) |
    ((cube->c & B_FACE_6) >> 6) |
    ((cube->c & B_FACE_7) >> 4) |
    (cube->c & ~B_FACE)
  );
}

static inline void R_face(Cube *cube) {
  cube->a = (
    ((cube->a & (R_FACE_0 | R_FACE_5)) << 2) |
    ((cube->a & R_FACE_1) << 4) |
    ((cube->a & R_FACE_2) << 6) |
    ((cube->a & (R_FACE_3 | R_FACE_8)) >> 2) |
    ((cube->a & R_FACE_6) >> 6) |
    ((cube->a & R_FACE_7) >> 4) |
    (cube->a & ~R_FACE)
  );
  cube->b = (
    ((cube->b & (R_FACE_0 | R_FACE_5)) << 2) |
    ((cube->b & R_FACE_1) << 4) |
    ((cube->b & R_FACE_2) << 6) |
    ((cube->b & (R_FACE_3 | R_FACE_8)) >> 2) |
    ((cube->b & R_FACE_6) >> 6) |
    ((cube->b & R_FACE_7) >> 4) |
    (cube->b & ~R_FACE)
  );
  cube->c = (
    ((cube->c & (R_FACE_0 | R_FACE_5)) << 2) |
    ((cube->c & R_FACE_1) << 4) |
    ((cube->c & R_FACE_2) << 6) |
    ((cube->c & (R_FACE_3 | R_FACE_8)) >> 2) |
    ((cube->c & R_FACE_6) >> 6) |
    ((cube->c & R_FACE_7) >> 4) |
    (cube->c & ~R_FACE)
  );
}

static inline void L_face_prime(Cube *cube) {
  cube->a = (
    ((cube->a & L_FACE_0) << 6) |
    ((cube->a & (L_FACE_1 | L_FACE_6)) << 2) |
    ((cube->a & (L_FACE_2 | L_FACE_7)) >> 2) |
    ((cube->a & L_FACE_3) << 4) |
    ((cube->a & L_FACE_5) >> 4) |
    ((cube->a & L_FACE_8) >> 6) |
    (cube->a & ~L_FACE)
  );
  cube->b = (
    ((cube->b & L_FACE_0) << 6) |
    ((cube->b & (L_FACE_1 | L_FACE_6)) << 2) |
    ((cube->b & (L_FACE_2 | L_FACE_7)) >> 2) |
    ((cube->b & L_FACE_3) << 4) |
    ((cube->b & L_FACE_5) >> 4) |
    ((cube->b & L_FACE_8) >> 6) |
    (cube->b & ~L_FACE)
  );
  cube->c = (
    ((cube->c & L_FACE_0) << 6) |
    ((cube->c & (L_FACE_1 | L_FACE_6)) << 2) |
    ((cube->c & (L_FACE_2 | L_FACE_7)) >> 2) |
    ((cube->c & L_FACE_3) << 4) |
    ((cube->c & L_FACE_5) >> 4) |
    ((cube->c & L_FACE_8) >> 6) |
    (cube->c & ~L_FACE)
  );
}

/* Elementary operations */

void turn_U_prime(Cube *cube) {
  // Sides
  cube->a = ((cube->a & U_BODY_MASK) << 9) | ((cube->a & U_TAIL_MASK) >> 27) | (cube->a & ~U_MASK);
  cube->b = ((cube->b & U_BODY_MASK) << 9) | ((cube->b & U_TAIL_MASK) >> 27) | (cube->b & ~U_MASK);
  cube->c = ((cube->c & U_BODY_MASK) << 9) | ((cube->c & U_TAIL_MASK) >> 27) | (cube->c & ~U_MASK);

  // Face
  U_face_prime(cube);
}

void rotate_y_prime(Cube *cube) {
  // Permute side faces
  cube->a = ((cube->a & Y_BODY_MASK) << 9) | ((cube->a & Y_TAIL_MASK) >> 27) | (cube->a & ~Y_MASK);
  cube->b = ((cube->b & Y_BODY_MASK) << 9) | ((cube->b & Y_TAIL_MASK) >> 27) | (cube->b & ~Y_MASK);
  cube->c = ((cube->c & Y_BODY_MASK) << 9) | ((cube->c & Y_TAIL_MASK) >> 27) | (cube->c & ~Y_MASK);

  // Rotate top face
  U_face_prime(cube);
  // Rotate bottom face
  D_face(cube);
}

void rotate_x(Cube *cube) {
  // Re-orient faces for wrapping
  U_face_prime(cube);
  U_face_prime(cube);
  B_face(cube);
  B_face(cube);
  // Permute ring faces
  cube->a = ((cube->a & X_1) << (3*9)) | ((cube->a & X_2) >> (4*9)) | ((cube->a & X_3) << (2*9)) | ((cube->a & X_0) >> 9) | (cube->a & ~X_MASK);
  cube->b = ((cube->b & X_1) << (3*9)) | ((cube->b & X_2) >> (4*9)) | ((cube->b & X_3) << (2*9)) | ((cube->b & X_0) >> 9) | (cube->b & ~X_MASK);
  cube->c = ((cube->c & X_1) << (3*9)) | ((cube->c & X_2) >> (4*9)) | ((cube->c & X_3) << (2*9)) | ((cube->c & X_0) >> 9) | (cube->c & ~X_MASK);

  // Rotate right face
  R_face(cube);
  // Rotate left face
  L_face_prime(cube);
}

/* Unoptimized basic operations */

void turn_U(Cube *cube) {
  turn_U_prime(cube);
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

// TODO:
/*
turn_D
turn_D_prime
turn_R
turn_R_prime
turn_L
turn_L_prime
turn_B
turn_B_prime

turn_U2
turn_D2
turn_R2
turn_L2
turn_F2
turn_B2

turn_u
turn_u_prime
turn_d
turn_d_prime
turn_r
turn_r_prime
turn_l
turn_l_prime
turn_f
turn_f_prime
turn_b
turn_b_prime

slice_M
slice_M_prime
slice_M2
slice_E
slice_E_prime
slice_E2
slice_S
slice_S_prime
slice_S2
*/

/* Helpers */

int color(Cube *cube, int index) {
  bitboard p = 1ULL << index;
  return !!(cube->a & p) + 2 * !!(cube->b & p) + 4 * !!(cube->c & p);
}

void render_raw(Cube *cube) {
  for (int j = 0; j < 6; ++j) {
    for (int i = 0; i < 9; ++i) {
      printf("\33[0;3%dm#", color(cube, i + 9 * j));
    }
    printf("\n");
  }
  printf("\33[0m");
  for (int i = 6*9; i < 64; ++i) {
    printf("%d", color(cube, i));
  }
  printf("\n");
}

void render(Cube *cube) {
  for (int j = 0; j < 3; ++j) {
    printf("   ");
    for (int i = 0; i < 3; ++i) {
      printf("\33[0;3%dm#", color(cube, i + j*3 + 4*9));
    }
    printf("\n");
  }
  for (int j = 0; j < 3; ++j) {
    for (int k = 0; k < 4; ++k) {
      for (int i = 0; i < 3; ++i) {
        printf("\33[0;3%dm#", color(cube, i + j*3 + k*9));
      }
    }
    printf("\n");
  }
  for (int j = 0; j < 3; ++j) {
    printf("   ");
    for (int i = 0; i < 3; ++i) {
      printf("\33[0;3%dm#", color(cube, i + j*3 + 5*9));
    }
    printf("\n");
  }

  printf("\33[0m");
}

int main() {
  Cube c = {0};
  reset(&c);
  // c.b ^= 123487987973483ULL;
  // c.c ^= 2323487987978923ULL;
  printf("Hello, cube!\n");
  render(&c);
  printf("Turned U\n");
  turn_U(&c);
  render(&c);
  printf("Rotated y\n");
  rotate_y(&c);
  render(&c);
  printf("Rotated x\n");
  rotate_x(&c);
  render(&c);
  printf("Turned F\n");
  turn_F(&c);
  render(&c);
  printf("Rotated Z'\n");
  rotate_z_prime(&c);
  render(&c);
  printf("Raw:\n");
  render_raw(&c);
  return 0;
}
