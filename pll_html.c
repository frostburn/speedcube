#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"
#include "math.h"
#include "assert.h"

#include "cube.c"
#include "moves.c"
#include "sequence.c"
#include "locdir.c"
#include "tablebase.c"
#include "goalsphere.c"
#include "ida_star.c"
#include "global_solver.c"

/* Display the F face as blue. */
char *jperm_sticker_fill(Cube *cube, bitboard p) {
  bitboard red = cube->a & ~cube->b & ~cube->c;
  bitboard green = ~cube->a & cube->b & ~cube->c;
  bitboard orange = cube->a & cube->b & ~cube->c;
  bitboard blue = ~cube->a & ~cube->b & cube->c;
  bitboard yellow = cube->a & ~cube->b & cube->c;
  if (p & red) {
    return "orange";
  }
  if (p & green) {
    return "blue";
  }
  if (p & orange) {
    return "red";
  }
  if (p & blue) {
    return "lime";
  }
  if (p & yellow) {
    return "yellow";
  }
  return "black";
}

int corner_loc_x(int loc) {
  switch (loc) {
    case 0:
      return 20;
    case 1:
      return 20;
    case 2:
      return 75;
    case 3:
      return 75;
  }
  exit(EXIT_FAILURE);
}

int corner_loc_y(int loc) {
  switch (loc) {
    case 0:
      return 20;
    case 1:
      return 77;
    case 2:
      return 77;
    case 3:
      return 20;
  }
  exit(EXIT_FAILURE);
}

int edge_loc_x(int loc) {
  switch (loc) {
    case 0:
      return 20;
    case 1:
      return 49;
    case 2:
      return 77;
    case 3:
      return 49;
  }
  exit(EXIT_FAILURE);
}

int edge_loc_y(int loc) {
  switch (loc) {
    case 0:
      return 49;
    case 1:
      return 77;
    case 2:
      return 49;
    case 3:
      return 20;
  }
  exit(EXIT_FAILURE);
}

void pll_svg(LocDirCube *ldc) {
  Cube cube = to_cube(ldc);
  bitboard p;
  char *fill;
  printf("<svg viewBox=\"0 0 98 98\" width=\"100%%\" height=\"100%%\">\n");

  printf(" <g stroke=\"black\" stroke-width=\"1.1\">\n");
  for (int i = 0; i < 3; ++i) {
    // B band
    p = 1ULL << (3*9 + 2 - i);
    fill = jperm_sticker_fill(&cube, p);
    printf("  <rect x=\"%g\" y=\"1\" width=\"25\" height=\"10\" fill=\"%s\" />\n", 10.5 + 26 * i, fill);
    // F band
    p = 1ULL << (9 + i);
    fill = jperm_sticker_fill(&cube, p);
    printf("  <rect x=\"%g\" y=\"87\" width=\"25\" height=\"10\" fill=\"%s\" />\n", 10.5 + 26 * i, fill);
    // L band
    p = 1ULL << i;
    fill = jperm_sticker_fill(&cube, p);
    printf("  <rect x=\"1\" y=\"%g\" width=\"10\" height=\"25\" fill=\"%s\" />\n", 10.5 + 26 * i, fill);
    // R band
    p = 1ULL << (2*9 + 2 - i);
    fill = jperm_sticker_fill(&cube, p);
    printf("  <rect x=\"87\" y=\"%g\" width=\"10\" height=\"25\" fill=\"%s\" />\n", 10.5 + 26 * i, fill);
  }
  printf(" </g>\n");

  printf(" <g stroke=\"black\" stroke-width=\"2.1\">\n");
  for (int j = 0; j < 3; ++j) {
    for (int i = 0; i < 3; ++i) {
      // U face
      p = 1ULL << (4*9 + i + 3*j);
      fill = jperm_sticker_fill(&cube, p);
      printf("  <rect x=\"%d\" y=\"%d\" width=\"24\" height=\"24\" fill=\"%s\" />\n", 11 + 26 * i, 11 + 26 * j, fill);
    }
  }
  printf(" </g>\n");

  printf(" <g stroke=\"black\" stroke-width=\"4\" fill=\"black\">\n");
  for (int to = 0; to < 4; ++to) {
    int from = ldc->corner_locs[to];
    if (to != from) {
      double x1 = corner_loc_x(from);
      double y1 = corner_loc_y(from);
      double x2 = corner_loc_x(to);
      double y2 = corner_loc_y(to);
      double u = x2 - x1;
      double v = y2 - y1;
      double r = hypot(u, v);
      u /= r;
      v /= r;
      x1 += u * 7;
      x2 -= u * 7;
      y1 += v * 7;
      y2 -= v * 7;
      printf("  <line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\" />\n", x1, y1, x2, y2);
      double tip_x = x2 + u * 4;
      double left_x = x2 - u * 2 - v * 4;
      double right_x = x2 - u * 2 + v * 4;
      double tip_y = y2 + v * 4;
      double left_y = y2 - v * 2 + u * 4;
      double right_y = y2 - v * 2 - u * 4;
      printf("  <path d=\"M %g,%g L %g,%g L %g,%g, z\" stroke=\"none\" />", left_x, left_y, tip_x, tip_y, right_x, right_y);
    }
  }
  for (int to = 0; to < 4; ++to) {
    int from = ldc->edge_locs[to];
    if (to != from) {
      double x1 = edge_loc_x(from);
      double y1 = edge_loc_y(from);
      double x2 = edge_loc_x(to);
      double y2 = edge_loc_y(to);
      double u = x2 - x1;
      double v = y2 - y1;
      double r = hypot(u, v);
      u /= r;
      v /= r;
      x1 += u * 7;
      x2 -= u * 7;
      y1 += v * 7;
      y2 -= v * 7;
      printf("  <line x1=\"%g\" y1=\"%g\" x2=\"%g\" y2=\"%g\" />\n", x1, y1, x2, y2);
      double tip_x = x2 + u * 4;
      double left_x = x2 - u * 2 - v * 4;
      double right_x = x2 - u * 2 + v * 4;
      double tip_y = y2 + v * 4;
      double left_y = y2 - v * 2 + u * 4;
      double right_y = y2 - v * 2 - u * 4;
      printf("  <path d=\"M %g,%g L %g,%g L %g,%g, z\" stroke=\"none\" />", left_x, left_y, tip_x, tip_y, right_x, right_y);
    }
  }
  printf(" </g>\n");

  printf("</svg>\n");
}

int main() {
  size_t search_depth = 4;
  prepare_global_solver();

  LocDirCube root;

  /*
  search_depth = 5;
  size_t radius = 7;
  fprintf(stderr, "Generating a goal sphere of radius %zu.\n", radius);
  free_goalsphere(&GLOBAL_SOLVER.goal);
  locdir_reset(&root);
  GLOBAL_SOLVER.goal = init_goalsphere(&root, radius, &locdir_centerless_hash);
  */


  char *names[] = {
    "Aa",
    "Aa y",
    "Aa y2",
    "Aa y'",

    "Ab",
    "Ab y",
    "Ab y2",
    "Ab y'",

    "F",
    "F y",
    "F y2",
    "F y'",

    "Ga",
    "Ga y",
    "Ga y2",
    "Ga y'",

    "Gb",
    "Gb y",
    "Gb y2",
    "Gb y'",

    "Gc",
    "Gc y",
    "Gc y2",
    "Gc y'",

    "Gd",
    "Gd y",
    "Gd y2",
    "Gd y'",

    "Ja",
    "Ja y",
    "Ja y2",
    "Ja y'",

    "Jb",
    "Jb y",
    "Jb y2",
    "Jb y'",

    "Ra",
    "Ra y",
    "Ra y2",
    "Ra y'",

    "Rb",
    "Rb y",
    "Rb y2",
    "Rb y'",

    "T",
    "T y",
    "T y2",
    "T y'",

    "E",
    "E y",

    "Na",

    "Nb",

    "V",
    "V y",
    "V y2",
    "V y'",

    "Y",
    "Y y",
    "Y y2",
    "Y y'",

    "H",

    "Ua",
    "Ua y",
    "Ua y2",
    "Ua y'",

    "Ub",
    "Ub y",
    "Ub y2",
    "Ub y'",

    "Z",
    "Z y",

    "O",
  };

  char *algos[] = {
    "f' U f' R2 f U' f' R2 f2", // Aa
    "R' F R' f2 r U' r' f2 R2",  // Aa y
    "R2 F2 R' f' U F2 U' f R'",  // Aa y2
    "F2 r2 f' U' f r2 F' R F'",  // Aa y'

    "r2 f2 R F R' B2 R F' R",  // Ab
    "R2 f2 r U r' f2 R F' R",  // Ab y
    "R f' U F2 U' f R F2 R2",  // Ab y2
    "f2 R2 f U f' R2 f U' f",  // Ab y'

    "U M U2 r' U R' F2 r F' R' M' F2 R2",  // F
    "U r2 f2 r R2 f' D r2 D' f R' U2 M'",  // F y
    "U M' U2 R' F r' F2 R U' R r2 F2 r2",  // F y2
    "U R2 f2 r' M U' R f2 R' U r' U2 M",  // F y'

    "U' f2 M2 U R2 U' R2 D R2 D' r2 f2",  // Ga
    "U' R2 S2 D f2 U' f2 U f2 D' F2 R2",  // Ga y
    "U' F2 M2 D R2 D' R2 U R2 U' r2 F2",  // Ga y2
    "U' r2 S2 U f2 D' f2 D f2 U' F2 r2",  // Ga y'

    "U R2 F2 U R2 D' R2 D f2 D' S2 R2",  // Gb
    "U F2 r2 D f2 D' f2 U R2 D' M2 F2",  // Gb y
    "U r2 F2 D r2 D' r2 U f2 U' S2 r2",  // Gb y2
    "U f2 r2 U F2 D' F2 D R2 U' M2 f2",  // Gb y'

    "U F2 M2 D' r2 U r2 U' r2 D R2 F2",  // Gc
    "U r2 S2 U' F2 U F2 D' F2 D f2 r2",  // Gc y
    "U f2 M2 U' r2 D r2 D' r2 U R2 f2",  // Gc y2
    "U R2 S2 D' F2 D F2 U' F2 U f2 R2",  // Gc y'

    "U' R2 f2 D' r2 D r2 U' F2 D S2 R2",  // Gd
    "U' F2 R2 U' F2 D F2 D' r2 D M2 F2",  // Gd y
    "U' f2 R2 D' f2 D f2 U' r2 U M2 f2",  // Gd y'
    "U' r2 f2 U' R2 D R2 D' F2 U S2 r2",  // Gd y2

    "l2 U R U' R f2 R' U R f2",  // Ja
    "b2 U F U' F R2 f' R f R2",  // Ja y
    "B2 R' U' R f2 R' U R' U' R2",  // Ja y2
    "f2 U f R' f R2 F' U F R2",  // Ja y'

    "R2 B U f' U2 F R' F R F2",  // Jb
    "F2 R U R' b2 R U' R U R2",  // Jb y
    "L2 F U F' r2 F U' F U F2",  // Jb y2
    "l2 U' R' U R' F2 r F' r' F2",  // Jb y'

    "U F U2 S R f' R' F U2 F2 U' F R",  // Ra
    "U R U2 M' f U' r' f R2 f2 r' U f",  // Ra y
    "U f R2 S' U F' r' U F2 U2 F' U r",  // Ra y2
    "U M F2 R2 f' U' f R2 F' R F' U2 M'",  // Ra y'

    "U F R U' R2 U2 R F' r' F M' U2 R",  // Rb
    "U F2 r2 f U2 r D r2 D' r' S U2 F",  // Rb y
    "U M' U2 f' U f' r2 f U' f' r2 f2 M",  // Rb y2
    "U R f R' f2 r2 F D' f' D S r2 f",  // Rb y'

    "U F2 U' F2 D R2 f2 D f2 D' R2",  // T
    "U r2 D' r2 D F2 R2 U R2 D' F2",  // T y
    "U f2 D' f2 D r2 f2 U f2 U' r2",  // T y2
    "U R2 U' R2 D f2 R2 D R2 U' f2",  // T y'

    "F U' f r2 F' R S' U' f r2 F' R f'",  // E
    "R U' r F2 r' U M' f' U F2 U' f r'",  // E y

    "R U2 f2 R f D' f D2 F' R F E2 r'",  // Na

    "R' U2 F2 r' D' r F' D2 f D' f' E2 r",  // Nb

    "R' U R' U' R D' R' D R' f2 D' f2 U R2",  // V
    "R U D2 r' F r U2 F2 D R D' F2 D2 R'",  // V y
    "R' U' F' S U F2 U' f' U2 r' F' R F' r",  // V y2
    "R U F S2 R' f2 r U f2 R f U' f r'",  // V y'

    "R' U' R F2 R' U R U F2 U' F2 U' F2",  // Y
    "F' U' F R2 f' R f U f2 U' f2 U' R2",  // Y y
    "R U R' F2 r F' r' U' r2 U r2 U F2",  // Y y'
    "R2 U' F2 D' F2 U F D F' R2 f R' f'",  // Y y2

    "M2 U M2 U2 M2 U M2",  // H

    "F2 U' M' U2 M U' F2",  // Ua
    "L2 d' M U2 M' U' f2",  // Ua y
    "M2 U M' U2 M U M2",  // Ua y2
    "R2 d' M' U2 M U' F2",  // Ua y'

    "F2 U M' U2 M U F2",  // Ub
    "L2 d M' U2 M U F2",  // Ub y
    "M2 U' M' U2 M U' M2",  // Ub y2
    "R2 d M U2 M' U f2",  // Ub y'

    "M S2 M' D' M2 u M2",  // Z
    "M S2 M' D M2 u' M2",  // Z y

    "",  // O
  };

  size_t num_names = sizeof(names)/sizeof(char*);
  size_t num_algos = sizeof(algos)/sizeof(char*);

  assert(num_names == num_algos);

  Cube cube;

  LocDirCube *cases = malloc(288 * sizeof(LocDirCube));
  size_t num_cases = 0;

  sequence Ga = parse("R2 U R' U R' U' R U' R2 U' D R' U R D'");

  void generate(LocDirCube *ldc, size_t depth) {
    cube = to_cube(ldc);
    if (is_yellow_permutation(&cube)) {
      for (size_t i = 0; i < num_cases; ++i) {
        if (locdir_equals(ldc, cases + i)) {
          return;
        }
      }
      cases[num_cases++] = *ldc;
    }
    if (depth <= 0) {
      return;
    }

    LocDirCube child = *ldc;
    locdir_U(&child);
    generate(&child, depth - 1);

    child = *ldc;
    locdir_F(&child);
    locdir_R(&child);
    locdir_U(&child);
    locdir_R_prime(&child);
    locdir_U_prime(&child);
    locdir_F_prime(&child);
    generate(&child, depth - 1);

    child = *ldc;
    locdir_apply_sequence(&child, Ga);
    generate(&child, depth - 1);
  }

  fprintf(stderr, "Generating PLL cases...\n");

  locdir_reset(&root);
  generate(&root, 14);

  fprintf(stderr, "%zu PLL cases generated\n", num_cases);

  LocDirCube ldc;
  sequence setup;
  sequence solution;
  char *solver;

  printf("<html>\n");
  printf("<head>\n");
  #if SCISSORS_ENABLED
  printf("<title>Shortest PLL algorithms (scissor turn metric)</title>\n");
  #else
  printf("<title>Shortest PLL algorithms (slice turn metric)</title>\n");
  #endif
  printf("<style>\n");
  printf("table, th, td {border: 1px solid;}\n");
  printf("td {text-align:center; padding:0.5em;}\n");
  printf("table {border-collapse: collapse;}\n");
  printf("td.case {width:100px; height:100px; padding:0;}\n");
  printf("</style>\n");
  printf("</head>\n");
  printf("<body>\n");
  printf("<p>PLL (Permutation of the Last Layer) solves the cube after <a href=\"oll.html\">OLL</a>.</p>\n");
  printf("<p>Shortest STM solutions discovered by <a href=\"https://github.com/frostburn/speedcube\">frostburn/speedcube</a>.</p>\n");
  printf("<p>Solutions obtained using IDA* are as short as possible, but not necessarily the easiest to perform.</p>\n");
  #if SCISSORS_ENABLED
  printf("<p>Scissor moves [in square brackets] can in principle be performed in one single motion.</p>");
  #endif
  printf("<table>\n");

  printf("<tr>\n");
  printf("<th>Name</th>\n");
  printf("<th>Case</th>\n");
  printf("<th>Algorithm</th>\n");
  printf("<th>Move count</th>\n");
  #if MAX_COMPLEXITY
  printf("<th>Move complexity</th>\n");
  #else
  printf("<th>Total complexity</th>\n");
  #endif
  printf("<th>Solver</th>\n");
  printf("</tr>\n");

  size_t total = 0;

  char *filename = malloc(256 * sizeof(char));

  for (size_t i = 0; i < num_algos; ++i) {
    fprintf(stderr, "Solving case %s\n", names[i]);
    setup = invert(parse(algos[i]));
    locdir_reset(&ldc);
    locdir_apply_sequence(&ldc, setup);
    // Solutions can rotate the final result, try to figure that out...
    if (ldc.center_locs[0] != 0) {
      locdir_reset(&ldc);
      locdir_y2(&ldc);
      locdir_apply_sequence(&ldc, setup);
    }
    if (ldc.center_locs[4] != 4) {
      locdir_reset(&ldc);
      locdir_x2(&ldc);
      locdir_apply_sequence(&ldc, setup);
    }
    if (ldc.center_locs[0] != 0) {
      locdir_reset(&ldc);
      locdir_z2(&ldc);
      locdir_apply_sequence(&ldc, setup);
    }
    if (ldc.center_locs[0] != 0) {
      locdir_reset(&ldc);
      locdir_z_prime(&ldc);
      locdir_apply_sequence(&ldc, setup);
    }
    if (ldc.center_locs[4] != 4) {
      locdir_reset(&ldc);
      locdir_x(&ldc);
      locdir_y2(&ldc);
      locdir_apply_sequence(&ldc, setup);
    }
    if (ldc.center_locs[4] != 4) {
      locdir_reset(&ldc);
      locdir_y2(&ldc);
      locdir_x(&ldc);
      locdir_apply_sequence(&ldc, setup);
    }
    if (ldc.center_locs[4] != 4) {
      locdir_reset(&ldc);
      locdir_x2(&ldc);
      locdir_y(&ldc);
      locdir_apply_sequence(&ldc, setup);
    }
    if (ldc.center_locs[0] != 0) {
      locdir_reset(&ldc);
      locdir_y_prime(&ldc);
      locdir_apply_sequence(&ldc, setup);
    }
    if (ldc.center_locs[4] != 4) {
      locdir_reset(&ldc);
      locdir_x2(&ldc);
      locdir_y_prime(&ldc);
      locdir_apply_sequence(&ldc, setup);
    }
    if (ldc.center_locs[0] != 0) {
      locdir_reset(&ldc);
      locdir_y(&ldc);
      locdir_apply_sequence(&ldc, setup);
    }
    assert(ldc.center_locs[0] == 0 && ldc.corner_locs[4] == 4);

    // Verify that generated cases match up with manual entries
    for (size_t k = 0; k < 4; ++k) {
      size_t j = 0;
      for (; j < num_cases; ++j) {
        if (locdir_equals(&ldc, cases + j)) {
          break;
        }
      }
      // cube = to_cube(&ldc);
      // render(&cube);
      assert(j < num_cases);
      for (; j < num_cases - 1; ++j) {
        cases[j] = cases[j + 1];
      }
      num_cases--;
      locdir_U(&ldc);
    }

    for (size_t k = 0; k < 4; ++k) {
      printf("<tr>\n");
      if (k == 0) {
        printf("<td>%s</td>\n", names[i]);
      } else if (k == 1) {
        printf("<td>%s U</td>\n", names[i]);
      } else if (k == 2) {
        printf("<td>%s U2</td>\n", names[i]);
      } else {
        printf("<td>%s U'</td>\n", names[i]);
      }
      printf("<td class=\"case\">\n");
      pll_svg(&ldc);
      printf("</td>\n");
      solver = "MitM";
      size_t depth = 0;
      for (; depth < search_depth; ++depth) {
        if (goalsphere_depth(&GLOBAL_SOLVER.goal, &ldc, depth) != UNKNOWN) {
          break;
        }
      }
      fprintf(stderr, "Depth = %zu + %zu\n", depth, GLOBAL_SOLVER.goal.num_sets - 1);
      sequence *solutions = NULL;
      solution = INVALID;
      solutions = goalsphere_solve_all(&GLOBAL_SOLVER.goal, &ldc, depth);
      fprintf(stderr, "Solutions @ %p\n", solutions);
      if (solutions == NULL) {
        solver = "IDA*";
        solutions = NULL;
        solution = global_solve(&ldc);
        fprint_sequence(stderr, solution);
        fprintf(stderr, "\nIDA*\n");
      } else {
        solution = INVALID;
        sequence* candidate = solutions;
        size_t num_solutions = 0;
        while (*candidate != SENTINEL) {
          solution = is_better(solution, *candidate) ? solution : *candidate;
          candidate++;
          num_solutions++;
        }
        fprint_sequence(stderr, solution);
        fprintf(stderr, "\n%zu solutions found.\n", num_solutions);
      }
      printf("<td>\n");
      if (solutions != NULL) {
        #if SCISSORS_ENABLED
        printf("<a href=\"txt/pll_scissors_%zu_%zu.txt\">\n", i, k);
        #else
        printf("<a href=\"txt/pll_%zu_%zu.txt\">\n", i, k);
        #endif
      }
      print_sequence(solution);
      if (solutions != NULL) {
        printf("</a>\n");
      }
      printf("</td>\n");
      printf("<td>%d</td>\n", sequence_length(solution));
      printf("<td>%d</td>\n", sequence_complexity(solution));
      printf("<td>%s</td>\n", solver);
      printf("</tr>\n");

      if (solutions != NULL) {
        #if SCISSORS_ENABLED
        sprintf(filename, "txt/pll_scissors_%zu_%zu.txt", i, k);
        #else
        sprintf(filename, "txt/pll_%zu_%zu.txt", i, k);
        #endif
        FILE *fptr = fopen(filename, "w");
        sequence *candidate = solutions;
        while (*candidate != SENTINEL) {
          fprint_sequence(fptr, *candidate);
          fprintf(fptr, "\n");
          candidate++;
        }
        fclose(fptr);

        free(solutions);
      }

      total++;
      locdir_U(&ldc);
    }
  }

  /*
  fprintf(stderr, "%zu cases remain\n", num_cases);
  size_t i = 0;
  while (num_cases) {
    fprintf(stderr, "Solving extra case %zu\n", i);

    ldc = cases[0];

    for (size_t k = 0; k < 4; ++k) {
      size_t j = 0;
      for (; j < num_cases; ++j) {
        if (locdir_equals(&ldc, cases + j)) {
          break;
        }
      }
      assert(j < num_cases);
      for (; j < num_cases - 1; ++j) {
        cases[j] = cases[j + 1];
      }
      num_cases--;

      printf("<tr>\n");
      printf("<td>%zu-%zu</td>", i, k);
      printf("<td class=\"case\">\n");
      pll_svg(&ldc);
      printf("</td>\n");
      fprintf(stderr, "Solving %zu-%zu with a goal sphere\n", i, k);
      solution = goalsphere_solve(&sphere, &ldc, search_depth);
      if (solution == INVALID) {
        fprintf(stderr, "Switching to IDA*\n");
        ida_star_solve(&GLOBAL_SOLVER.ida, &ldc);
        solution = ida_to_sequence(&GLOBAL_SOLVER.ida);
      }
      printf("<td>\n");
      print_sequence(solution);
      printf("</td>\n");
      printf("</tr>\n");
      locdir_U(&ldc);
      total++;
    }
    i++;
  }
  */

  // Make sure that everything was solved.
  assert(num_cases == 0);

  printf("</table>\n");

  printf("<p>%zu algorithms in total.</p>\n", total);

  printf("</body>\n");
  printf("</html>\n");

  free(cases);
  free(filename);
  free_global_solver();

  return EXIT_SUCCESS;
}
