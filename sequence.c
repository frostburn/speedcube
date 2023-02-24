typedef unsigned __int128 sequence;

#define SEQUENCE_MAX_LENGTH (23)

#define MAX_COMPLEXITY 1

const sequence NOTHING = 0;
const sequence INVALID = ~NOTHING;

sequence from_moves(enum move *moves) {
  sequence result = 0;
  for(;;) {
    enum move m = *moves;
    if (m == I) {
      return result;
    }
    result = NUM_MOVES * result + m;
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
    #if MAX_COMPLEXITY
    score_a = score_a > move_a ? score_a : move_a;
    score_b = score_b > move_b ? score_b : move_b;
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

int sequence_complexity(sequence seq) {
  int result = 0;
  for (int i = 0; i < SEQUENCE_MAX_LENGTH; ++i) {
    int m = seq % NUM_MOVES;
    #if MAX_COMPLEXITY
    result = result > m ? result : m;
    #else
    result += m;
    #endif
    seq /= NUM_MOVES;
  }
  return result;
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

sequence parse(char *string) {
  if (string[0] == '\0') {
    return I;
  }
  if (string[0] == ' ') {
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

#define NUM_FACE_TURNS (18)

enum move FACE_TURNS[] = {
  U, U_prime, U2,
  D, D_prime, D2,
  R, R_prime, R2,
  L, L_prime, L2,
  F, F_prime, F2,
  B, B_prime, B2,
};

sequence make_scramble(Cube *root, int length) {
  if (length > SEQUENCE_MAX_LENGTH) {
    fprintf(stderr, "Desired sramble length too long");
    exit(EXIT_FAILURE);
  }

  Cube path[SEQUENCE_MAX_LENGTH + 1];
  path[0] = *root;

  sequence result = I;
  int index = 1;
  int face_bucket_2 = -1;
  int face_bucket = -1;
  while (index <= length) {
    int turn_index = rand() % NUM_FACE_TURNS;
    int commuting_index = turn_index / 6;
    if (turn_index / 3 == face_bucket || (face_bucket / 2 == commuting_index && face_bucket_2 == turn_index / 3)) {
      continue;
    }
    face_bucket_2 = face_bucket;
    face_bucket = turn_index / 3;
    enum move m = FACE_TURNS[turn_index];

    path[index] = path[index - 1];
    apply(path + index, m);
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
    result = m + result * NUM_MOVES;
    index++;
  }
  return result;
}
