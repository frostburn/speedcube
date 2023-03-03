#define SCISSORS_ENABLED 0

#if SCISSORS_ENABLED
#define NUM_MOVES (64)
#else
#define NUM_MOVES (46)
#endif

// In priority order
enum move {
  I,

  U, U_prime,
  R, R_prime,

  M_prime,

  F,

  U2, R2,
  M2, F2,
  F_prime,

  r, r_prime, r2,
  u, u_prime, u2,
  f, f_prime, f2,

  D, D_prime, D2,

  L, L_prime, L2,

  l, l_prime, l2,
  d, d_prime, d2,

  M,

  B, B_prime, B2,

  E, E2, E_prime,

  S, S_prime, S2,
  b, b_prime, b2,

  UD, UpDp,
  FB, FpBp,

  U2D, U2Dp, D2U, D2Up,
  F2B, F2Bp, B2F, B2Fp,

  RL, RpLp,
  R2L, R2Lp, L2R, L2Rp,
};

#if SCISSORS_ENABLED
const enum move MAX_MOVE = L2Rp;
#else
const enum move MAX_MOVE = b2;
#endif

/* Bitboard masks */
const bitboard U_BODY = 7 | (7 << 9) | (7 << 18);
const bitboard U_TAIL = 7 << 27;
const bitboard U_MASK = ~(U_BODY | U_TAIL);
const bitboard U_FACE_0 = 1ULL << (4*9);
const bitboard U_FACE_1 = 1ULL << (4*9 + 1);
const bitboard U_FACE_2 = 1ULL << (4*9 + 2);
const bitboard U_FACE_3 = 1ULL << (4*9 + 3);
const bitboard U_FACE_5 = 1ULL << (4*9 + 5);
const bitboard U_FACE_6 = 1ULL << (4*9 + 6);
const bitboard U_FACE_7 = 1ULL << (4*9 + 7);
const bitboard U_FACE_8 = 1ULL << (4*9 + 8);
const bitboard U_FACE_MASK = ~(U_FACE_0 | U_FACE_1 | U_FACE_2 | U_FACE_3 | U_FACE_5 | U_FACE_6 | U_FACE_7 | U_FACE_8);

const bitboard E_BODY = 56 | (56 << 9) | (56 << 18);
const bitboard E_TAIL = 56ULL << 27;
const bitboard E_MASK = ~(E_BODY | E_TAIL);

const bitboard M_0 = 146ULL << (4*9);
const bitboard M_1 = 146ULL << (1*9);
const bitboard M_2_0 = 1ULL << (5*9 + 1);
const bitboard M_2_1 = 1ULL << (5*9 + 4);
const bitboard M_2_2 = 1ULL << (5*9 + 7);
const bitboard M_3_0 = 1ULL << (3*9 + 1);
const bitboard M_3_1 = 1ULL << (3*9 + 4);
const bitboard M_3_2 = 1ULL << (3*9 + 7);
const bitboard M_MASK = ~(M_0 | M_1 | M_2_0 | M_2_1 | M_2_2 | M_3_0 | M_3_1 | M_3_2);

const bitboard D_FACE_0 = 1ULL << (5*9);
const bitboard D_FACE_1 = 1ULL << (5*9 + 1);
const bitboard D_FACE_2 = 1ULL << (5*9 + 2);
const bitboard D_FACE_3 = 1ULL << (5*9 + 3);
const bitboard D_FACE_5 = 1ULL << (5*9 + 5);
const bitboard D_FACE_6 = 1ULL << (5*9 + 6);
const bitboard D_FACE_7 = 1ULL << (5*9 + 7);
const bitboard D_FACE_8 = 1ULL << (5*9 + 8);
const bitboard D_FACE_MASK = ~(D_FACE_0 | D_FACE_1 | D_FACE_2 | D_FACE_3 | D_FACE_5 | D_FACE_6 | D_FACE_7 | D_FACE_8);

const bitboard B_FACE_0 = 1ULL << (3*9);
const bitboard B_FACE_1 = 1ULL << (3*9 + 1);
const bitboard B_FACE_2 = 1ULL << (3*9 + 2);
const bitboard B_FACE_3 = 1ULL << (3*9 + 3);
const bitboard B_FACE_5 = 1ULL << (3*9 + 5);
const bitboard B_FACE_6 = 1ULL << (3*9 + 6);
const bitboard B_FACE_7 = 1ULL << (3*9 + 7);
const bitboard B_FACE_8 = 1ULL << (3*9 + 8);
const bitboard B_FACE_MASK = ~(B_FACE_0 | B_FACE_1 | B_FACE_2 | B_FACE_3 | B_FACE_5 | B_FACE_6 | B_FACE_7 | B_FACE_8);

const bitboard R_0_0 = 1ULL << (4*9 + 2);
const bitboard R_0_1 = 1ULL << (4*9 + 5);
const bitboard R_0_2 = 1ULL << (4*9 + 8);
const bitboard R_1 = 292ULL << (1*9);
const bitboard R_2 = 292ULL << (5*9);
const bitboard R_3_0 = 1ULL << (3*9);
const bitboard R_3_1 = 1ULL << (3*9 + 3);
const bitboard R_3_2 = 1ULL << (3*9 + 6);
const bitboard R_MASK = ~(R_0_0 | R_0_1 | R_0_2 | R_1 | R_2 | R_3_0 | R_3_1 | R_3_2);
const bitboard R_FACE_0 = 1ULL << (2*9);
const bitboard R_FACE_1 = 1ULL << (2*9 + 1);
const bitboard R_FACE_2 = 1ULL << (2*9 + 2);
const bitboard R_FACE_3 = 1ULL << (2*9 + 3);
const bitboard R_FACE_4 = 1ULL << (2*9 + 4);
const bitboard R_FACE_5 = 1ULL << (2*9 + 5);
const bitboard R_FACE_6 = 1ULL << (2*9 + 6);
const bitboard R_FACE_7 = 1ULL << (2*9 + 7);
const bitboard R_FACE_8 = 1ULL << (2*9 + 8);
const bitboard R_FACE_MASK = ~(R_FACE_0 | R_FACE_1 | R_FACE_2 | R_FACE_3 | R_FACE_5 | R_FACE_6 | R_FACE_7 | R_FACE_8);

const bitboard L_FACE_0 = 1ULL;
const bitboard L_FACE_1 = 1ULL << 1;
const bitboard L_FACE_2 = 1ULL << 2;
const bitboard L_FACE_3 = 1ULL << 3;
const bitboard L_FACE_4 = 1ULL << 4;
const bitboard L_FACE_5 = 1ULL << 5;
const bitboard L_FACE_6 = 1ULL << 6;
const bitboard L_FACE_7 = 1ULL << 7;
const bitboard L_FACE_8 = 1ULL << 8;
const bitboard L_FACE_MASK = ~(L_FACE_0 | L_FACE_1 | L_FACE_2 | L_FACE_3 | L_FACE_5 | L_FACE_6 | L_FACE_7 | L_FACE_8);

const bitboard Y_BODY = 511ULL | (511ULL << 9) | (511ULL << 18);
const bitboard Y_TAIL = 511ULL << 27;
const bitboard Y_MASK = ~(Y_BODY | Y_TAIL);

const bitboard X_0_0 = 1ULL << (4*9);
const bitboard X_0_1 = 1ULL << (4*9 + 1);
const bitboard X_0_2 = 1ULL << (4*9 + 2);
const bitboard X_0_3 = 1ULL << (4*9 + 3);
const bitboard X_0_4 = 1ULL << (4*9 + 4);
const bitboard X_0_5 = 1ULL << (4*9 + 5);
const bitboard X_0_6 = 1ULL << (4*9 + 6);
const bitboard X_0_7 = 1ULL << (4*9 + 7);
const bitboard X_0_8 = 1ULL << (4*9 + 8);
const bitboard X_1 = 511ULL << (1*9);
const bitboard X_2 = 511ULL << (5*9);
const bitboard X_3_0 = 1ULL << (3*9);
const bitboard X_3_1 = 1ULL << (3*9 + 1);
const bitboard X_3_2 = 1ULL << (3*9 + 2);
const bitboard X_3_3 = 1ULL << (3*9 + 3);
const bitboard X_3_4 = 1ULL << (3*9 + 4);
const bitboard X_3_5 = 1ULL << (3*9 + 5);
const bitboard X_3_6 = 1ULL << (3*9 + 6);
const bitboard X_3_7 = 1ULL << (3*9 + 7);
const bitboard X_3_8 = 1ULL << (3*9 + 8);
const bitboard X_MASK = R_FACE_4 | L_FACE_4;

/* Face rotations */

static inline bitboard U_face_prime(bitboard component) {
  return (
    ((component & U_FACE_0) << 6) |
    ((component & (U_FACE_1 | U_FACE_6)) << 2) |
    ((component & (U_FACE_2 | U_FACE_7)) >> 2) |
    ((component & U_FACE_3) << 4) |
    ((component & U_FACE_5) >> 4) |
    ((component & U_FACE_8) >> 6) |
    (component & U_FACE_MASK)
  );
}

static inline bitboard D_face(bitboard component) {
  return (
    ((component & (D_FACE_0 | D_FACE_5)) << 2) |
    ((component & D_FACE_1) << 4) |
    ((component & D_FACE_2) << 6) |
    ((component & (D_FACE_3 | D_FACE_8)) >> 2) |
    ((component & D_FACE_6) >> 6) |
    ((component & D_FACE_7) >> 4) |
    (component & D_FACE_MASK)
  );
}

static inline bitboard B_face(bitboard component) {
  return (
    ((component & (B_FACE_0 | B_FACE_5)) << 2) |
    ((component & B_FACE_1) << 4) |
    ((component & B_FACE_2) << 6) |
    ((component & (B_FACE_3 | B_FACE_8)) >> 2) |
    ((component & B_FACE_6) >> 6) |
    ((component & B_FACE_7) >> 4) |
    (component & B_FACE_MASK)
  );
}

static inline bitboard R_side(bitboard component) {
  return (
    ((component & R_1) << (3*9)) |
    ((component & R_2) >> (4*9)) |
    ((component & R_0_0) >> (9 - 4)) | ((component & R_0_1) >> (9 + 2)) | ((component & R_0_2) >> (9 + 8)) |
    ((component & R_3_0) << (2*9 + 8)) | ((component & R_3_1) << (2*9 + 2)) | ((component & R_3_2) << (2*9 - 4)) |
    (component & R_MASK)
  );
}

static inline bitboard R_face_maskless(bitboard component) {
  return (
    ((component & (R_FACE_0 | R_FACE_5)) << 2) |
    ((component & R_FACE_1) << 4) |
    ((component & R_FACE_2) << 6) |
    ((component & (R_FACE_3 | R_FACE_8)) >> 2) |
    ((component & R_FACE_6) >> 6) |
    ((component & R_FACE_7) >> 4)
  );
}

static inline bitboard L_face_prime_maskless(bitboard component) {
  return (
    ((component & L_FACE_0) << 6) |
    ((component & (L_FACE_1 | L_FACE_6)) << 2) |
    ((component & (L_FACE_2 | L_FACE_7)) >> 2) |
    ((component & L_FACE_3) << 4) |
    ((component & L_FACE_5) >> 4) |
    ((component & L_FACE_8) >> 6)
  );
}

static inline bitboard M_ring(bitboard component) {
  return(
    ((component & M_0) >> (3*9)) |
    ((component & M_1) << (4*9)) |
    ((component & M_2_0) >> (2*9 - 6)) | ((component & M_2_1) >> (2*9)) | ((component & M_2_2) >> (2*9 +6)) |
    ((component & M_3_0) << (9 + 6)) | ((component & M_3_1) << 9) | ((component & M_3_2) << (9 - 6)) |
    (component & M_MASK)
  );
}

static inline bitboard X_ring_maskless(bitboard component) {
  return (
    ((component & X_1) << (3*9)) |
    ((component & X_2) >> (4*9)) |
    ((component & X_3_0) << (2*9 + 8)) | ((component & X_3_1) << (2*9 + 6)) | ((component & X_3_2) << (2*9 + 4)) |
    ((component & X_3_3) << (2*9 + 2)) | ((component & X_3_4) << (2*9)) | ((component & X_3_5) << (2*9 - 2)) |
    ((component & X_3_6) << (2*9 - 4)) | ((component & X_3_7) << (2*9 - 6)) | ((component & X_3_8) << (2*9 - 8)) |
    ((component & X_0_0) >> (9 - 8)) | ((component & X_0_1) >> (9 - 6)) | ((component & X_0_2) >> (9 - 4)) |
    ((component & X_0_3) >> (9 - 2)) | ((component & X_0_4) >> 9) | ((component & X_0_5) >> (9 + 2)) |
    ((component & X_0_6) >> (9 + 4)) | ((component & X_0_7) >> (9 + 6)) | ((component & X_0_8) >> (9 + 8))
  );
}

/* Elementary operations */

void turn_U_prime(Cube *cube) {
  // Sides
  cube->a = ((cube->a & U_BODY) << 9) | ((cube->a & U_TAIL) >> 27) | (cube->a & U_MASK);
  cube->b = ((cube->b & U_BODY) << 9) | ((cube->b & U_TAIL) >> 27) | (cube->b & U_MASK);
  cube->c = ((cube->c & U_BODY) << 9) | ((cube->c & U_TAIL) >> 27) | (cube->c & U_MASK);

  // Face
  cube->a = U_face_prime(cube->a);
  cube->b = U_face_prime(cube->b);
  cube->c = U_face_prime(cube->c);
}

void turn_R(Cube *cube) {
  // Sides
  cube->a = R_side(cube->a);
  cube->b = R_side(cube->b);
  cube->c = R_side(cube->c);

  cube->a = R_face_maskless(cube->a) | (cube->a & R_FACE_MASK);
  cube->b = R_face_maskless(cube->b) | (cube->b & R_FACE_MASK);
  cube->c = R_face_maskless(cube->c) | (cube->c & R_FACE_MASK);
}

void slice_E(Cube *cube) {
  cube->a = ((cube->a & E_BODY) << 9) | ((cube->a & E_TAIL) >> 27) | (cube->a & E_MASK);
  cube->b = ((cube->b & E_BODY) << 9) | ((cube->b & E_TAIL) >> 27) | (cube->b & E_MASK);
  cube->c = ((cube->c & E_BODY) << 9) | ((cube->c & E_TAIL) >> 27) | (cube->c & E_MASK);
}

void slice_M(Cube *cube) {
 cube->a = M_ring(cube->a);
 cube->b = M_ring(cube->b);
 cube->c = M_ring(cube->c);
}

// TODO: Eliminate masks from face rotations
void rotate_y_prime(Cube *cube) {
  // Permute side faces
  cube->a = ((cube->a & Y_BODY) << 9) | ((cube->a & Y_TAIL) >> 27) | (cube->a & Y_MASK);
  cube->b = ((cube->b & Y_BODY) << 9) | ((cube->b & Y_TAIL) >> 27) | (cube->b & Y_MASK);
  cube->c = ((cube->c & Y_BODY) << 9) | ((cube->c & Y_TAIL) >> 27) | (cube->c & Y_MASK);

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
  cube->a = X_ring_maskless(cube->a) | R_face_maskless(cube->a) | L_face_prime_maskless(cube->a) | (cube->a & X_MASK);
  cube->b = X_ring_maskless(cube->b) | R_face_maskless(cube->b) | L_face_prime_maskless(cube->b) | (cube->b & X_MASK);
  cube->c = X_ring_maskless(cube->c) | R_face_maskless(cube->c) | L_face_prime_maskless(cube->c) | (cube->c & X_MASK);
}

/* Unoptimized basic operations */

void rotate_y(Cube *cube) {
  rotate_y_prime(cube);
  rotate_y_prime(cube);
  rotate_y_prime(cube);
}
void rotate_y2(Cube *cube) {
  rotate_y_prime(cube);
  rotate_y_prime(cube);
}

void rotate_x_prime(Cube *cube) {
  rotate_x(cube);
  rotate_x(cube);
  rotate_x(cube);
}
void rotate_x2(Cube *cube) {
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
void rotate_z2(Cube *cube) {
  rotate_y_prime(cube);
  rotate_x2(cube);
  rotate_y(cube);
}

void turn_U(Cube *cube) {
  turn_U_prime(cube);
  turn_U_prime(cube);
  turn_U_prime(cube);
}
void turn_U2(Cube *cube) {
  turn_U_prime(cube);
  turn_U_prime(cube);
}

void turn_D(Cube *cube) {
  rotate_x2(cube);
  turn_U(cube);
  rotate_x2(cube);
}
void turn_D_prime(Cube *cube) {
  rotate_x2(cube);
  turn_U_prime(cube);
  rotate_x2(cube);
}
void turn_D2(Cube *cube) {
  rotate_x2(cube);
  turn_U2(cube);
  rotate_x2(cube);
}

void turn_R_prime(Cube *cube) {
  turn_R(cube);
  turn_R(cube);
  turn_R(cube);
}
void turn_R2(Cube *cube) {
  turn_R(cube);
  turn_R(cube);
}

void turn_L(Cube *cube) {
  rotate_y2(cube);
  turn_R(cube);
  rotate_y2(cube);
}
void turn_L_prime(Cube *cube) {
  rotate_y2(cube);
  turn_R_prime(cube);
  rotate_y2(cube);
}
void turn_L2(Cube *cube) {
  rotate_y2(cube);
  turn_R2(cube);
  rotate_y2(cube);
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

void turn_B(Cube *cube) {
  rotate_x_prime(cube);
  turn_U(cube);
  rotate_x(cube);
}
void turn_B_prime(Cube *cube) {
  rotate_x_prime(cube);
  turn_U_prime(cube);
  rotate_x(cube);
}
void turn_B2(Cube *cube) {
  rotate_x_prime(cube);
  turn_U2(cube);
  rotate_x(cube);
}

void turn_u(Cube *cube) {
  rotate_y(cube);
  turn_D(cube);
}
void turn_u_prime(Cube *cube) {
  rotate_y_prime(cube);
  turn_D_prime(cube);
}
void turn_u2(Cube *cube) {
  rotate_y2(cube);
  turn_D2(cube);
}

void turn_d(Cube *cube) {
  rotate_y_prime(cube);
  turn_U(cube);
}
void turn_d_prime(Cube *cube) {
  rotate_y(cube);
  turn_U_prime(cube);
}
void turn_d2(Cube *cube) {
  rotate_y2(cube);
  turn_U2(cube);
}

void turn_r(Cube *cube) {
  rotate_x(cube);
  turn_L(cube);
}
void turn_r_prime(Cube *cube) {
  rotate_x_prime(cube);
  turn_L_prime(cube);
}
void turn_r2(Cube *cube) {
  rotate_x2(cube);
  turn_L2(cube);
}

void turn_l(Cube *cube) {
  rotate_x_prime(cube);
  turn_R(cube);
}
void turn_l_prime(Cube *cube) {
  rotate_x(cube);
  turn_R_prime(cube);
}
void turn_l2(Cube *cube) {
  rotate_x2(cube);
  turn_R2(cube);
}

void turn_f(Cube *cube) {
  rotate_z(cube);
  turn_B(cube);
}
void turn_f_prime(Cube *cube) {
  rotate_z_prime(cube);
  turn_B_prime(cube);
}
void turn_f2(Cube *cube) {
  rotate_z2(cube);
  turn_B2(cube);
}

void turn_b(Cube *cube) {
  rotate_z_prime(cube);
  turn_F(cube);
}
void turn_b_prime(Cube *cube) {
  rotate_z(cube);
  turn_F_prime(cube);
}
void turn_b2(Cube *cube) {
  rotate_z2(cube);
  turn_F2(cube);
}

void slice_E_prime(Cube *cube) {
  slice_E(cube);
  slice_E(cube);
  slice_E(cube);
}
void slice_E2(Cube *cube) {
  slice_E(cube);
  slice_E(cube);
}

void slice_M_prime(Cube *cube) {
  slice_M(cube);
  slice_M(cube);
  slice_M(cube);
}
void slice_M2(Cube *cube) {
  slice_M(cube);
  slice_M(cube);
}

void slice_S(Cube *cube) {
  rotate_x_prime(cube);
  slice_E(cube);
  rotate_x(cube);
}
void slice_S_prime(Cube *cube) {
  rotate_x_prime(cube);
  slice_E_prime(cube);
  rotate_x(cube);
}
void slice_S2(Cube *cube) {
  rotate_x_prime(cube);
  slice_E2(cube);
  rotate_x(cube);
}

void apply(Cube *cube, enum move move) {
  switch (move) {
    case I:
      break;
    case U:
      turn_U(cube);
      break;
    case U_prime:
      turn_U_prime(cube);
      break;
    case U2:
      turn_U2(cube);
      break;
    case D:
      turn_D(cube);
      break;
    case D_prime:
      turn_D_prime(cube);
      break;
    case D2:
      turn_D2(cube);
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
    case L:
      turn_L(cube);
      break;
    case L_prime:
      turn_L_prime(cube);
      break;
    case L2:
      turn_L2(cube);
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
    case B:
      turn_B(cube);
      break;
    case B_prime:
      turn_B_prime(cube);
      break;
    case B2:
      turn_B2(cube);
      break;
    case u:
      turn_u(cube);
      break;
    case u_prime:
      turn_u_prime(cube);
      break;
    case u2:
      turn_u2(cube);
      break;
    case d:
      turn_d(cube);
      break;
    case d_prime:
      turn_d_prime(cube);
      break;
    case d2:
      turn_d2(cube);
      break;
    case r:
      turn_r(cube);
      break;
    case r_prime:
      turn_r_prime(cube);
      break;
    case r2:
      turn_r2(cube);
      break;
    case l:
      turn_l(cube);
      break;
    case l_prime:
      turn_l_prime(cube);
      break;
    case l2:
      turn_l2(cube);
      break;
    case f:
      turn_f(cube);
      break;
    case f_prime:
      turn_f_prime(cube);
      break;
    case f2:
      turn_f2(cube);
      break;
    case b:
      turn_b(cube);
      break;
    case b_prime:
      turn_b_prime(cube);
      break;
    case b2:
      turn_b2(cube);
      break;
    case M:
      slice_M(cube);
      break;
    case M_prime:
      slice_M_prime(cube);
      break;
    case M2:
      slice_M2(cube);
      break;
    case E:
      slice_E(cube);
      break;
    case E_prime:
      slice_E_prime(cube);
      break;
    case E2:
      slice_E2(cube);
      break;
    case S:
      slice_S(cube);
      break;
    case S_prime:
      slice_S_prime(cube);
      break;
    case S2:
      slice_S2(cube);
      break;
    case UD:
      turn_U(cube);
      turn_D(cube);
      break;
    case UpDp:
      turn_U_prime(cube);
      turn_D_prime(cube);
      break;
    case U2D:
      turn_U2(cube);
      turn_D(cube);
      break;
    case U2Dp:
      turn_U2(cube);
      turn_D_prime(cube);
      break;
    case D2U:
      turn_D2(cube);
      turn_U(cube);
      break;
    case D2Up:
      turn_D2(cube);
      turn_U_prime(cube);
      break;
    case FB:
      turn_F(cube);
      turn_B(cube);
      break;
    case FpBp:
      turn_F_prime(cube);
      turn_B_prime(cube);
      break;
    case F2B:
      turn_F2(cube);
      turn_B(cube);
      break;
    case F2Bp:
      turn_F2(cube);
      turn_B_prime(cube);
      break;
    case B2F:
      turn_B2(cube);
      turn_F(cube);
      break;
    case B2Fp:
      turn_B2(cube);
      turn_F_prime(cube);
      break;
    case RL:
      turn_R(cube);
      turn_L(cube);
      break;
    case RpLp:
      turn_R_prime(cube);
      turn_L_prime(cube);
      break;
    case R2L:
      turn_R2(cube);
      turn_L(cube);
      break;
    case R2Lp:
      turn_R2(cube);
      turn_L_prime(cube);
      break;
    case L2R:
      turn_L2(cube);
      turn_R(cube);
      break;
    case L2Rp:
      turn_L2(cube);
      turn_R_prime(cube);
      break;
    default:
      fprintf(stderr, "Unimplemented move\n");
      exit(EXIT_FAILURE);
  }
}

/* Compensate applied move with a rotation to keep the centers in place. */
void compensate(Cube *cube, enum move move) {
  switch (move) {
    case u:
      rotate_y_prime(cube);
      break;
    case u_prime:
      rotate_y(cube);
      break;
    case u2:
      rotate_y2(cube);
      break;
    case d:
      rotate_y(cube);
      break;
    case d_prime:
      rotate_y_prime(cube);
      break;
    case d2:
      rotate_y2(cube);
      break;
    case r:
      rotate_x_prime(cube);
      break;
    case r_prime:
      rotate_x(cube);
      break;
    case r2:
      rotate_x2(cube);
      break;
    case l:
      rotate_x(cube);
      break;
    case l_prime:
      rotate_x_prime(cube);
      break;
    case l2:
      rotate_x2(cube);
      break;
    case f:
      rotate_z_prime(cube);
      break;
    case f_prime:
      rotate_z(cube);
      break;
    case f2:
      rotate_z2(cube);
      break;
    case b:
      rotate_z(cube);
      break;
    case b_prime:
      rotate_z_prime(cube);
      break;
    case b2:
      rotate_z2(cube);
      break;
    case M:
      rotate_x(cube);
      break;
    case M_prime:
      rotate_x_prime(cube);
      break;
    case M2:
      rotate_x2(cube);
      break;
    case E:
      rotate_y(cube);
      break;
    case E_prime:
      rotate_y_prime(cube);
      break;
    case E2:
      rotate_y2(cube);
      break;
    case S:
      rotate_z_prime(cube);
      break;
    case S_prime:
      rotate_z(cube);
      break;
    case S2:
      rotate_z2(cube);
      break;
    default:
      break;
  }
}

/* Cube utilities */

void scramble(Cube *cube) {
  for (int i = 0; i < 100; ++i) {
    int r = rand() % 6;
    switch(r) {
      case 0:
        turn_U(cube);
        break;
      case 1:
        turn_D(cube);
        break;
      case 2:
        turn_R(cube);
        break;
      case 3:
        turn_L(cube);
        break;
      case 4:
        turn_F(cube);
        break;
      case 5:
        turn_B(cube);
        break;
    }
  }
}

void roll(Cube *cube) {
  for (int i = 0; i < 100; ++i) {
    int r = rand() % 2;
    switch(r) {
      case 0:
        rotate_y_prime(cube);
        break;
      case 1:
        rotate_x(cube);
        break;
    }
  }
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
