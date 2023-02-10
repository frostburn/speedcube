typedef unsigned __int128 sequence;

const int SEQUENCE_MAX_LENGTH = 23;

const sequence NOTHING = 0;
const sequence INVALID = ~NOTHING;

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
      case d:
        printf("d ");
        break;
      case d_prime:
        printf("d' ");
        break;
      case d2:
        printf("d2 ");
      case r:
        printf("r ");
        break;
      case r_prime:
        printf("r' ");
        break;
      case r2:
        printf("r2 ");
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
