typedef unsigned __int128 sequence;

#define SEQUENCE_MAX_LENGTH (23)

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

void print_sequence(sequence seq) {
  if (seq == INVALID) {
    printf("<DNF>\n");
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
        printf("U ");
        break;
      case U_prime:
        printf("U' ");
        break;
      case U2:
        printf("U2 ");
        break;
      case D:
        printf("D ");
        break;
      case D_prime:
        printf("D' ");
        break;
      case D2:
        printf("D2 ");
        break;
      case R:
        printf("R ");
        break;
      case R_prime:
        printf("R' ");
        break;
      case R2:
        printf("R2 ");
        break;
      case L:
        printf("L ");
        break;
      case L_prime:
        printf("L' ");
        break;
      case L2:
        printf("L2 ");
        break;
      case F:
        printf("F ");
        break;
      case F_prime:
        printf("F' ");
        break;
      case F2:
        printf("F2 ");
        break;
      case B:
        printf("B ");
        break;
      case B_prime:
        printf("B' ");
        break;
      case B2:
        printf("B2 ");
        break;
      case u:
        printf("u ");
        break;
      case u_prime:
        printf("u' ");
        break;
      case u2:
        printf("u2 ");
        break;
      case d:
        printf("d ");
        break;
      case d_prime:
        printf("d' ");
        break;
      case d2:
        printf("d2 ");
        break;
      case r:
        printf("r ");
        break;
      case r_prime:
        printf("r' ");
        break;
      case r2:
        printf("r2 ");
        break;
      case l:
        printf("l ");
        break;
      case l_prime:
        printf("l' ");
        break;
      case l2:
        printf("l2 ");
        break;
      case f:
        printf("f ");
        break;
      case f_prime:
        printf("f' ");
        break;
      case f2:
        printf("f2 ");
        break;
      case b:
        printf("b ");
        break;
      case b_prime:
        printf("b' ");
        break;
      case b2:
        printf("b2 ");
        break;
      case M:
        printf("M ");
        break;
      case M_prime:
        printf("M' ");
        break;
      case M2:
        printf("M2 ");
        break;
      case E:
        printf("E ");
        break;
      case E_prime:
        printf("E' ");
        break;
      case E2:
        printf("E2 ");
        break;
      case S:
        printf("S ");
        break;
      case S_prime:
        printf("S' ");
        break;
      case S2:
        printf("S2 ");
        break;
      case I:
        break;
      default:
        printf("? ");
        break;
    }
    seq /= NUM_MOVES;
  }
  printf("\n");
}

bool is_better(sequence a, sequence b) {
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
    score_a += move_a;
    score_b += move_b;
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
    result += seq % NUM_MOVES;
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
