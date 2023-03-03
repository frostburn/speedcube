#include "math.h"

typedef unsigned __int128 sequence;

#if SCISSORS_ENABLED
#define SEQUENCE_MAX_LENGTH (21)
#else
#define SEQUENCE_MAX_LENGTH (23)
#endif

#define POW_COMPLEXITY 1

const sequence NOTHING = 0;
const sequence INVALID = ~NOTHING;

sequence from_moves(enum move *moves) {
  sequence result = 0;
  for(;;) {
    enum move move = *moves;
    if (move == I) {
      return result;
    }
    result = NUM_MOVES * result + move;
    moves++;
  }
}

sequence reverse(sequence seq) {
  sequence result = 0;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    sequence move = seq % NUM_MOVES;
    if (move) {
      result = move + result * NUM_MOVES;
    }
    seq /= NUM_MOVES;
  }
  return result;
}

sequence concat(sequence a, sequence b) {
  if (a == INVALID || b == INVALID) {
    return INVALID;
  }
  sequence reversed = 0;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    reversed = reversed * NUM_MOVES + b % NUM_MOVES;
    b /= NUM_MOVES;
  }
  b = reversed;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    sequence move = b % NUM_MOVES;
    if (move != I) {
      a = a * NUM_MOVES + move;
    }
    b /= NUM_MOVES;
  }
  return a;
}

sequence invert(sequence seq) {
  sequence result = 0;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    sequence move = seq % NUM_MOVES;
    switch(move) {
      case U:
        move = U_prime;
        break;
      case U_prime:
        move = U;
        break;
      case D:
        move = D_prime;
        break;
      case D_prime:
        move = D;
        break;
      case R:
        move = R_prime;
        break;
      case R_prime:
        move = R;
        break;
      case L:
        move = L_prime;
        break;
      case L_prime:
        move = L;
        break;
      case F:
        move = F_prime;
        break;
      case F_prime:
        move = F;
        break;
      case B:
        move = B_prime;
        break;
      case B_prime:
        move = B;
        break;
      case u:
        move = u_prime;
        break;
      case u_prime:
        move = u;
        break;
      case d:
        move = d_prime;
        break;
      case d_prime:
        move = d;
        break;
      case r:
        move = r_prime;
        break;
      case r_prime:
        move = r;
        break;
      case l:
        move = l_prime;
        break;
      case l_prime:
        move = l;
        break;
      case f:
        move = f_prime;
        break;
      case f_prime:
        move = f;
        break;
      case b:
        move = b_prime;
        break;
      case b_prime:
        move = b;
        break;
      case M:
        move = M_prime;
        break;
      case M_prime:
        move = M;
        break;
      case E:
        move = E_prime;
        break;
      case E_prime:
        move = E;
        break;
      case S:
        move = S_prime;
        break;
      case S_prime:
        move = S;
        break;
      case UD:
        move = UpDp;
        break;
      case UpDp:
        move = UD;
        break;
      case U2D:
        move = U2Dp;
        break;
      case U2Dp:
        move = U2D;
        break;
      case D2U:
        move = D2Up;
        break;
      case D2Up:
        move = D2U;
        break;
      case FB:
        move = FpBp;
        break;
      case FpBp:
        move = FB;
        break;
      case F2B:
        move = F2Bp;
        break;
      case F2Bp:
        move = F2B;
        break;
      case B2F:
        move = B2Fp;
        break;
      case B2Fp:
        move = B2F;
        break;
      case RL:
        move = RpLp;
        break;
      case RpLp:
        move = RL;
        break;
      case R2L:
        move = R2Lp;
        break;
      case R2Lp:
        move = R2L;
        break;
      case L2R:
        move = L2Rp;
        break;
      case L2Rp:
        move = L2R;
        break;
    }
    if (move != I) {
      result = result * NUM_MOVES + move;
    }
    seq /= NUM_MOVES;
  }
  return result;
}

void apply_sequence(Cube *cube, sequence seq) {
  sequence reversed = 0;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    reversed = reversed * NUM_MOVES + seq % NUM_MOVES;
    seq /= NUM_MOVES;
  }
  seq = reversed;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    apply(cube, seq % NUM_MOVES);
    seq /= NUM_MOVES;
  }
}

void fprint_sequence(FILE *file, sequence seq) {
  if (seq == INVALID) {
    fprintf(file, "<DNF>");
    return;
  }
  sequence reversed = 0;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    reversed = reversed * NUM_MOVES + seq % NUM_MOVES;
    seq /= NUM_MOVES;
  }
  seq = reversed;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    switch (seq % NUM_MOVES) {
      case U:
        fprintf(file, "U ");
        break;
      case U_prime:
        fprintf(file, "U' ");
        break;
      case U2:
        fprintf(file, "U2 ");
        break;
      case D:
        fprintf(file, "D ");
        break;
      case D_prime:
        fprintf(file, "D' ");
        break;
      case D2:
        fprintf(file, "D2 ");
        break;
      case R:
        fprintf(file, "R ");
        break;
      case R_prime:
        fprintf(file, "R' ");
        break;
      case R2:
        fprintf(file, "R2 ");
        break;
      case L:
        fprintf(file, "L ");
        break;
      case L_prime:
        fprintf(file, "L' ");
        break;
      case L2:
        fprintf(file, "L2 ");
        break;
      case F:
        fprintf(file, "F ");
        break;
      case F_prime:
        fprintf(file, "F' ");
        break;
      case F2:
        fprintf(file, "F2 ");
        break;
      case B:
        fprintf(file, "B ");
        break;
      case B_prime:
        fprintf(file, "B' ");
        break;
      case B2:
        fprintf(file, "B2 ");
        break;
      case u:
        fprintf(file, "u ");
        break;
      case u_prime:
        fprintf(file, "u' ");
        break;
      case u2:
        fprintf(file, "u2 ");
        break;
      case d:
        fprintf(file, "d ");
        break;
      case d_prime:
        fprintf(file, "d' ");
        break;
      case d2:
        fprintf(file, "d2 ");
        break;
      case r:
        fprintf(file, "r ");
        break;
      case r_prime:
        fprintf(file, "r' ");
        break;
      case r2:
        fprintf(file, "r2 ");
        break;
      case l:
        fprintf(file, "l ");
        break;
      case l_prime:
        fprintf(file, "l' ");
        break;
      case l2:
        fprintf(file, "l2 ");
        break;
      case f:
        fprintf(file, "f ");
        break;
      case f_prime:
        fprintf(file, "f' ");
        break;
      case f2:
        fprintf(file, "f2 ");
        break;
      case b:
        fprintf(file, "b ");
        break;
      case b_prime:
        fprintf(file, "b' ");
        break;
      case b2:
        fprintf(file, "b2 ");
        break;
      case M:
        fprintf(file, "M ");
        break;
      case M_prime:
        fprintf(file, "M' ");
        break;
      case M2:
        fprintf(file, "M2 ");
        break;
      case E:
        fprintf(file, "E ");
        break;
      case E_prime:
        fprintf(file, "E' ");
        break;
      case E2:
        fprintf(file, "E2 ");
        break;
      case S:
        fprintf(file, "S ");
        break;
      case S_prime:
        fprintf(file, "S' ");
        break;
      case S2:
        fprintf(file, "S2 ");
        break;
      case UD:
        fprintf(file, "[UD] ");
        break;
      case UpDp:
        fprintf(file, "[U'D'] ");
        break;
      case U2D:
        fprintf(file, "[U2D] ");
        break;
      case U2Dp:
        fprintf(file, "[U2D'] ");
        break;
      case D2U:
        fprintf(file, "[D2U] ");
        break;
      case D2Up:
        fprintf(file, "[D2U'] ");
        break;
      case FB:
        fprintf(file, "[FB] ");
        break;
      case FpBp:
        fprintf(file, "[F'B'] ");
        break;
      case F2B:
        fprintf(file, "[F2B] ");
        break;
      case F2Bp:
        fprintf(file, "[F2B'] ");
        break;
      case B2F:
        fprintf(file, "[B2F] ");
        break;
      case B2Fp:
        fprintf(file, "[B2F'] ");
        break;
      case RL:
        fprintf(file, "[RL] ");
        break;
      case RpLp:
        fprintf(file, "[R'L'] ");
        break;
      case R2L:
        fprintf(file, "[R2L] ");
        break;
      case R2Lp:
        fprintf(file, "[R2L'] ");
        break;
      case L2R:
        fprintf(file, "[L2R] ");
        break;
      case L2Rp:
        fprintf(file, "[L2R'] ");
        break;
      case I:
        break;
      default:
        fprintf(file, "? ");
        break;
    }
    seq /= NUM_MOVES;
  }
}

void print_sequence(sequence seq) {
  fprint_sequence(stdout, seq);
  printf("\n");
}

bool is_better(sequence a, sequence b) {
  if (a == INVALID) {
    return false;
  }
  if (b == INVALID) {
    return true;
  }
  bool lexicographic = (a < b);
  int score_a = 0;
  int score_b = 0;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    int move_a = a % NUM_MOVES;
    int move_b = b % NUM_MOVES;
    // Shorter
    if (!move_a && move_b) {
      return true;
    }
    // Longer
    if (move_a && !move_b) {
      return false;
    }
    #if POW_COMPLEXITY
    move_a *= move_a;
    score_a += move_a*move_a;
    move_b *= move_b;
    score_b += move_b*move_b;
    #else
    score_a += move_a;
    score_b += move_b;
    #endif
    a /= NUM_MOVES;
    b /= NUM_MOVES;
  }
  // Simpler
  if (score_a < score_b) {
    return true;
  }
  // More complex
  if (score_a > score_b) {
    return false;
  }
  // Use lexicographic order
  return lexicographic;
}

int semistable_score(enum move move) {
  switch(move) {
    case I:
      return 0;
    case U:
      return 1;
    case U_prime:
      return 2;
    case R:
      return 3;
    case R_prime:
      return 4;
    case F:
      return 5;
    case F_prime:
      return 6;
    case U2:
      return 7;
    case R2:
      return 8;
    case F2:
      return 9;
    case D:
      return 10;
    case D_prime:
      return 11;
    case D2:
      return 12;
    case L:
      return 13;
    case L_prime:
      return 14;
    case L2:
      return 15;
    case B:
      return 16;
    case B_prime:
      return 17;
    case B2:
      return 18;
    case M_prime:
      return 19;
    case M2:
      return 20;
    case M:
      return 21;
    case S:
      return 22;
    case S_prime:
      return 23;
    case E_prime:
      return 24;
    case E:
      return 25;
    case S2:
      return 26;
    case E2:
      return 27;
    case UD:
      return 28;
    case UpDp:
      return 29;
    case FB:
      return 30;
    case FpBp:
      return 31;
    case U2D:
      return 32;
    case U2Dp:
      return 33;
    case D2U:
      return 34;
    case D2Up:
      return 35;
    case F2B:
      return 36;
    case F2Bp:
      return 37;
    case B2F:
      return 38;
    case B2Fp:
      return 39;
    case RL:
      return 40;
    case RpLp:
      return 41;
    case R2L:
      return 42;
    case R2Lp:
      return 43;
    case L2R:
      return 44;
    case L2Rp:
      return 45;
    default:
      return 45 + move;
  }
}

bool is_better_semistable(sequence a, sequence b) {
  if (a == INVALID) {
    return false;
  }
  if (b == INVALID) {
    return true;
  }
  bool lexicographic = (a < b);
  int score_a = 0;
  int score_b = 0;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    int move_a = a % NUM_MOVES;
    int move_b = b % NUM_MOVES;
    // Shorter
    if (!move_a && move_b) {
      return true;
    }
    // Longer
    if (move_a && !move_b) {
      return false;
    }
    move_a = semistable_score(move_a);
    move_b = semistable_score(move_b);
    #if POW_COMPLEXITY
    move_a *= move_a;
    score_a += move_a*move_a;
    move_b *= move_b;
    score_b += move_b*move_b;
    #else
    score_a += move_a;
    score_b += move_b;
    #endif

    a /= NUM_MOVES;
    b /= NUM_MOVES;
  }
  // Simpler
  if (score_a < score_b) {
    return true;
  }
  // More complex
  if (score_a > score_b) {
    return false;
  }
  // Use lexicographic order
  return lexicographic;
}

double sequence_complexity(sequence seq) {
  double result = 0;
  double length = 0;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    int m = seq % NUM_MOVES;
    if (m) {
      length++;
    }
    #if POW_COMPLEXITY
    result += m*m*m*m;
    #else
    result += m;
    #endif
    seq /= NUM_MOVES;
  }
  result /= length;
  #if POW_COMPLEXITY
  return pow(result, 0.25);
  #else
  return result;
  #endif
}

int sequence_length(sequence seq) {
  int result = 0;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    if (seq % NUM_MOVES) {
      result++;
    }
    seq /= NUM_MOVES;
  }
  return result;
}

enum move parse_move(char chr) {
  switch (chr) {
    case 'U':
      return U;
    case 'D':
      return D;
    case 'R':
      return R;
    case 'L':
      return L;
    case 'F':
      return F;
    case 'B':
      return B;
    case 'u':
      return u;
    case 'd':
      return d;
    case 'r':
      return r;
    case 'l':
      return l;
    case 'f':
      return f;
    case 'b':
      return b;
    case 'M':
      return M;
    case 'E':
      return E;
    case 'S':
      return S;
  }
  fprintf(stderr, "Unrecognized character %c\n", chr);
  exit(EXIT_FAILURE);
}

enum move parse_prime(char chr) {
  switch (chr) {
    case 'U':
      return U_prime;
    case 'D':
      return D_prime;
    case 'R':
      return R_prime;
    case 'L':
      return L_prime;
    case 'F':
      return F_prime;
    case 'B':
      return B_prime;
    case 'u':
      return u_prime;
    case 'd':
      return d_prime;
    case 'r':
      return r_prime;
    case 'l':
      return l_prime;
    case 'f':
      return f_prime;
    case 'b':
      return b_prime;
    case 'M':
      return M_prime;
    case 'E':
      return E_prime;
    case 'S':
      return S_prime;
  }
  fprintf(stderr, "Unrecognized character %c\n", chr);
  exit(EXIT_FAILURE);
}

enum move parse_double(char chr) {
  switch (chr) {
    case 'U':
      return U2;
    case 'D':
      return D2;
    case 'R':
      return R2;
    case 'L':
      return L2;
    case 'F':
      return F2;
    case 'B':
      return B2;
    case 'u':
      return u2;
    case 'd':
      return d2;
    case 'r':
      return r2;
    case 'l':
      return l2;
    case 'f':
      return f2;
    case 'b':
      return b2;
    case 'M':
      return M2;
    case 'E':
      return E2;
    case 'S':
      return S2;
  }
  fprintf(stderr, "Unrecognized character %c\n", chr);
  exit(EXIT_FAILURE);
}

// TODO: Parse scissor moves into their respective singles
sequence parse(char *string) {
  if (string[0] == '\0') {
    return I;
  }
  if (string[0] == ' ' || string[0] == '[' || string[0] == ']') {
    return parse(string + 1);
  }
  if (string[1] == '\'') {
    return concat(parse_prime(string[0]), parse(string + 2));
  }
  if (string[1] == '2') {
    return concat(parse_double(string[0]), parse(string + 2));
  }
  return concat(parse_move(string[0]), parse(string + 1));
}

void apply_char(Cube *cube, char chr) {
  switch (chr) {
    case 'x':
      rotate_x(cube);
      break;
    case 'y':
      rotate_y(cube);
      break;
    case 'z':
      rotate_z(cube);
      break;
    default:
      apply(cube, parse_move(chr));
      break;
  }
}

void apply_char_prime(Cube *cube, char chr) {
  switch (chr) {
    case 'x':
      rotate_x_prime(cube);
      break;
    case 'y':
      rotate_y_prime(cube);
      break;
    case 'z':
      rotate_z_prime(cube);
      break;
    default:
      apply(cube, parse_prime(chr));
      break;
  }
}

void apply_char_double(Cube *cube, char chr) {
  switch (chr) {
    case 'x':
      rotate_x2(cube);
      break;
    case 'y':
      rotate_y2(cube);
      break;
    case 'z':
      rotate_z2(cube);
      break;
    default:
      apply(cube, parse_double(chr));
      break;
  }
}

void apply_string(Cube *cube, char *string) {
  if (string[0] == '\0') {
    return;
  }
  if (string[0] == ' '  || string[0] == '[' || string[0] == ']') {
    apply_string(cube, string + 1);
  } else if (string[1] == '\'') {
    apply_char_prime(cube, string[0]);
    apply_string(cube, string + 2);
  } else if (string[1] == '2') {
    apply_char_double(cube, string[0]);
    apply_string(cube, string + 2);
  } else {
    apply_char(cube, string[0]);
    apply_string(cube, string + 1);
  }
}

#define NUM_FACE_TURNS (18)

enum move FACE_TURNS[] = {
  U, U_prime, U2,
  D, D_prime, D2,
  R, R_prime, R2,
  L, L_prime, L2,
  F, F_prime, F2,
  B, B_prime, B2,
};

// TODO: Fix! This produced F L2 R' R' D2 R' D2 L' F' with R' repeated.
sequence make_scramble(Cube *root, int length) {
  if (length > SEQUENCE_MAX_LENGTH) {
    fprintf(stderr, "Desired sramble length too long");
    exit(EXIT_FAILURE);
  }

  Cube path[SEQUENCE_MAX_LENGTH + 1];
  path[0] = *root;

  sequence result = I;
  int index = 1;
  int prev_prev_face_bucket = -1;
  int prev_face_bucket = -1;
  while (index <= length) {
    int turn_index = rand() % NUM_FACE_TURNS;
    int face_bucket = turn_index / 3;
    int commuting_bucket = face_bucket / 2;
    int prev_commuting_bucket = prev_face_bucket / 2;
    if (face_bucket == prev_face_bucket || (commuting_bucket == prev_commuting_bucket && prev_prev_face_bucket == face_bucket)) {
      continue;
    }
    prev_prev_face_bucket = prev_face_bucket;
    prev_face_bucket = face_bucket;

    enum move move = FACE_TURNS[turn_index];

    path[index] = path[index - 1];
    apply(path + index, move);
    bool in_path = false;
    for (int i = 0; i < index; ++i) {
      if (equals(path + i, path + index)) {
        in_path = true;
        break;
      }
    }
    if (in_path) {
      continue;
    }
    result = move + result * NUM_MOVES;
    index++;
  }
  return result;
}
