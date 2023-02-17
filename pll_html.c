#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"
#include "math.h"

#include "cube.c"
#include "moves.c"
#include "sequence.c"
#include "bst.c"
#include "indexer.c"
#include "locdir.c"
#include "tablebase.c"
#include "goalsphere.c"
#include "ida_star.c"
#include "global_solver.c"

char *sticker_fill(Cube *cube, bitboard p) {
  bitboard red = cube->a & ~cube->b & ~cube->c;
  bitboard green = ~cube->a & cube->b & ~cube->c;
  bitboard orange = cube->a & cube->b & ~cube->c;
  bitboard blue = ~cube->a & ~cube->b & cube->c;
  bitboard yellow = cube->a & ~cube->b & cube->c;
  bitboard white = ~cube->a & cube->b & cube->c;
  bitboard purple = cube->a & cube->b & cube->c;
  if (p & red) {
    return "red";
  }
  if (p & green) {
    return "lime";
  }
  if (p & orange) {
    return "orange";
  }
  if (p & blue) {
    return "blue";
  }
  if (p & yellow) {
    return "yellow";
  }
  if (p & white) {
    return "white";
  }
  if (p & purple) {
    return "purple";
  }
  return "black";
}

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
  prepare_global_solver();

  LocDirCube root;
  fprintf(stderr, "Generating a PLL goal sphere of radius 6.\n");
  locdir_reset(&root);
  GoalSphere sphere = init_goalsphere(&root, 6, &locdir_centerless_hash);

  char *names[] = {
    "Aa",
    "Ab",
    "F",
    "Ga",
    "Gb",
    "Gc",
    "Gd",
    "Ja",
    "Jb",
    "Ra",
    "Rb",
    "T",
    "E",
    "Na",
    "Nb",
    "V",
    "Y",
    "H",
    "Ua",
    "Ua'",
    "Ub",
    "Ub'",
    "Z",
  };

  char *algos[] = {
    /*x*/ "L2 D2 L' U' L D2 L' U L'" /*x'*/,
    /*x'*/ "L2 D2 L U L' D2 L U' L" /*x*/,
    "R' U' F' R U R' U' R' F R2 U' R' U' R U R' U R",
    "R2 U R' U R' U' R U' R2 U' D R' U R D'",
    "R' U' R U D' R2 U R' U R U' R U' R2 D",
    "R2 U' R U' R U R' U R2 U D' R U' R' D",
    "R U R' U' D R2 U' R U' R' U R' U R2 D'",
    /*x*/ "R2 F R F' R U2 r' U r U2" /*x'*/,
    "R U R' F' R U R' U' R' F R2 U' R'",
    "R U' R' U' R U R D R' U' R D' R' U2 R'",
    "R2 F R U R U' R' F' R U2 R' U2 R",
    "R U R' U' R' F R2 U' R' U' R U R' F'",
    /*x'*/ "L' U L D' L' U' L D L' U' L D' L' U L D",
    "R U R' U R U R' F' R U R' U' R' F R2 U' R' U2 R U' R'",
    "R' U R U' R' F' U' F R U R' F R' F' R U' R",
    /*R' U R' U' y*/ "R' F' R2 U' R' U R' F R F",
    "F R U' R' U' R U R' F' R U R' U' R' F R F'",
    "M2 U M2 U2 M2 U M2",
    "M2 U M U2 M' U M2",
    "M2 U M' U2 M U M2",
    "M2 U' M U2 M' U' M2",
    "L2 d M' U2 M U F2",
    "M' U M2 U M2 U M' U2 M2",
  };

  /*
  LocDirCube *cases = malloc(96 * sizeof(LocDirCube));
  size_t num_cases = 0;

  void generate(LocDirCube *ldc, size_t depth) {
    Cube cube = to_cube(ldc);
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
  }

  locdir_reset(&root);
  generate(&root, 20);

  fprintf(stderr, "%zu PLL cases generated\n", num_cases);
  */

  Cube cube;
  LocDirCube ldc;
  sequence setup;
  sequence solution;
  size_t search_depth = 3;

  printf("<html>\n");
  printf("<head>\n");
  printf("<title>Shortest PLL algorithms (slice turn metric)</title>\n");
  printf("<style>\n");
  printf("table, th, td {border: 1px solid;}\n");
  printf("table {border-collapse: collapse;}\n");
  printf("td.case {width:100px; height:100px;}\n");
  printf("</style>\n");
  printf("</head>\n");
  printf("<body>\n");
  printf("<p>PLL (Permutation of the Last Layer) solves the cube after <a href=\"oll.html\">OLL</a>.</p>\n");
  printf("<p>Shortest STM solutions discovered by <a href=\"https://github.com/frostburn/speedcube\">frostburn/speedcube</a>.</p>\n");
  printf("<table>\n");

  printf("<tr>\n");
  printf("<th>Name</th>\n");
  printf("<th>Case</th>\n");
  printf("<th>Algorithm</th>\n");
  printf("</tr>\n");

  size_t total = 0;

  // for (size_t i = 0; i < num_cases; ++i) {
  for (size_t i = 0; i < 23; ++i) {
    fprintf(stderr, "Solving case %s\n", names[i]);
    setup = invert(parse(algos[i]));
    locdir_reset(&ldc);
    if (i == 0) {
      locdir_x(&ldc);
    } else if (i == 1) {
      locdir_x_prime(&ldc);
    } else if (i == 7) {
      locdir_x(&ldc);
    } else if (i == 12) {
      locdir_x_prime(&ldc);
    } else if (i == 15) {
      locdir_y(&ldc);
    }
    locdir_apply_sequence(&ldc, setup);
    if (i == 0) {
      locdir_x_prime(&ldc);
    } else if (i == 1) {
      locdir_x(&ldc);
    } else if (i == 7) {
      locdir_x_prime(&ldc);
    } else if (i == 12) {
      locdir_x(&ldc);
    } else if (i == 15) {
      locdir_y_prime(&ldc);
      locdir_U(&ldc);
      locdir_R(&ldc);
      locdir_U_prime(&ldc);
      locdir_R(&ldc);
    }

    // ldc = cases[i];

    printf("<tr>\n");
    printf("<td>%s</td>\n", names[i]);
    // printf("<td>%zu</td>", i);
    printf("<td class=\"case\">\n");
    pll_svg(&ldc);
    printf("</td>\n");
    solution = goalsphere_solve(&sphere, &ldc, search_depth);
    if (solution == INVALID) {
      ida_star_solve(&GLOBAL_SOLVER.ida, &ldc);
      solution = ida_to_sequence(&GLOBAL_SOLVER.ida);
    }
    printf("<td>\n");
    print_sequence(solution);
    printf("</td>\n");
    printf("</tr>\n");

    total++;

    locdir_U(&ldc);
    printf("<tr>\n");
    printf("<td>%s U</td>\n", names[i]);
    printf("<td class=\"case\">\n");
    pll_svg(&ldc);
    printf("</td>\n");
    solution = goalsphere_solve(&sphere, &ldc, search_depth);
    if (solution == INVALID) {
      ida_star_solve(&GLOBAL_SOLVER.ida, &ldc);
      solution = ida_to_sequence(&GLOBAL_SOLVER.ida);
    }
    printf("<td>\n");
    print_sequence(solution);
    printf("</td>\n");
    printf("</tr>\n");

    total++;

    locdir_U(&ldc);
    printf("<tr>\n");
    printf("<td>%s U2</td>\n", names[i]);
    printf("<td class=\"case\">\n");
    pll_svg(&ldc);
    printf("</td>\n");
    solution = goalsphere_solve(&sphere, &ldc, search_depth);
    if (solution == INVALID) {
      ida_star_solve(&GLOBAL_SOLVER.ida, &ldc);
      solution = ida_to_sequence(&GLOBAL_SOLVER.ida);
    }
    printf("<td>\n");
    print_sequence(solution);
    printf("</td>\n");
    printf("</tr>\n");

    total++;

    locdir_U(&ldc);
    printf("<tr>\n");
    printf("<td>%s U'</td>\n", names[i]);
    printf("<td class=\"case\">\n");
    pll_svg(&ldc);
    printf("</td>\n");
    solution = goalsphere_solve(&sphere, &ldc, search_depth);
    if (solution == INVALID) {
      ida_star_solve(&GLOBAL_SOLVER.ida, &ldc);
      solution = ida_to_sequence(&GLOBAL_SOLVER.ida);
    }
    printf("<td>\n");
    print_sequence(solution);
    printf("</td>\n");
    printf("</tr>\n");

    total++;
  }

  printf("</table>\n");

  printf("<p>%zu algorithms in total.</p>\n", total);

  printf("</body>\n");
  printf("</html>\n");

  free_goalsphere(&sphere);
  // free(cases);
  free_global_solver();

  return EXIT_SUCCESS;
}
