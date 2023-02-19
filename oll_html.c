#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"
#include "assert.h"

#include "cube.c"
#include "moves.c"
#include "sequence.c"
#include "locdir.c"
#include "tablebase.c"
#include "goalsphere.c"
#include "ida_star.c"

void oll_svg(Cube *cube) {
  bitboard yellow = cube->a & ~cube->b & cube->c;
  bitboard p;
  char *fill;
  printf("<svg viewBox=\"0 0 98 98\" width=\"100%%\" height=\"100%%\">\n");

  printf(" <g fill=\"gray\" stroke=\"black\" stroke-width=\"1.1\">\n");
  for (int i = 0; i < 3; ++i) {
    // B band
    p = 1ULL << (3*9 + 2 - i);
    fill = (yellow & p) ? "yellow" : "gray";
    printf("  <rect x=\"%g\" y=\"1\" width=\"25\" height=\"10\" fill=\"%s\" />\n", 10.5 + 26 * i, fill);
    // F band
    p = 1ULL << (9 + i);
    fill = (yellow & p) ? "yellow" : "gray";
    printf("  <rect x=\"%g\" y=\"87\" width=\"25\" height=\"10\" fill=\"%s\" />\n", 10.5 + 26 * i, fill);
    // L band
    p = 1ULL << i;
    fill = (yellow & p) ? "yellow" : "gray";
    printf("  <rect x=\"1\" y=\"%g\" width=\"10\" height=\"25\" fill=\"%s\" />\n", 10.5 + 26 * i, fill);
    // R band
    p = 1ULL << (2*9 + 2 - i);
    fill = (yellow & p) ? "yellow" : "gray";
    printf("  <rect x=\"87\" y=\"%g\" width=\"10\" height=\"25\" fill=\"%s\" />\n", 10.5 + 26 * i, fill);
  }
  printf(" </g>\n");

  printf(" <g stroke=\"black\" stroke-width=\"2.1\">\n");
  for (int j = 0; j < 3; ++j) {
    for (int i = 0; i < 3; ++i) {
      // U face
      p = 1ULL << (4*9 + i + 3*j);
      fill = (yellow & p) ? "yellow" : "gray";
      printf("  <rect x=\"%d\" y=\"%d\" width=\"24\" height=\"24\" fill=\"%s\" />\n", 11 + 26 * i, 11 + 26 * j, fill);
    }
  }
  printf(" </g>\n");

  printf("</svg>\n");
}

int main() {
  size_t radius = 7;
  size_t search_depth = 4;

  LocDirCube root;
  fprintf(stderr, "Generating an OLL goal sphere of radius %zu.\n", radius);
  locdir_reset(&root);
  GoalSphere sphere = init_goalsphere(&root, radius, &locdir_oll_index);

  char *algos[] = {
    "",
    "R U2 R2 F R F' U2 R' F R F'",
    "r U r' U2 r U2 R' U2 R U' r'",
    "r' R2 U R' U r U2 r' U M'",
    "M U' r U2 r' U' R U' R' M'",
    "l' U2 L U L' U l",
    "r U2 R' U' R U' r'",
    "r U R' U R U2 r'",
    "l' U' L U' L' U2 l",
    "R U R' U' R' F R2 U R' U' F'",
    "R U R' U R' F R F' R U2 R'",
    "r U R' U R' F R F' R U2 r'",
    "M' R' U' R U' R' U2 R U' R r'",
    "F U R U' R2 F' R U R U' R'",
    "R' F R U R' F' R F U' F'",
    "l' U' l L' U' L U l' U l",
    "r U r' R U R' U' r U' r'",
    "F R' F' R2 r' U R U' R' U' M'",
    "r U R' U R U2 r2 U' R U' R' U2 r",
    "r' R U R U R' U' M' R' F R F'",
    "r U R' U' M2 U R U' R' U' M'",
    "R U2 R' U' R U R' U' R U' R'",
    "R U2 R2 U' R2 U' R2 U2 R",
    "R2 D' R U2 R' D R U2 R",
    "r U R' U' r' F R F'",
    "F' r U R' U' r' F R",
    "R U2 R' U' R U' R'",
    "R U R' U R U2 R'",
    "r U R' U' r' R U R U' R'",
    "R U R' U' R U' R' F' U' F R U R'",
    "F R' F R2 U' R' U' R U R' F2",
    "R' U' F U R U' R' F' R",
    "L U F' U' L' U L F L'",
    "R U R' U' R' F R F'",
    "R U R2 U' R' F R U R U' F'",
    "R U2 R2 F R F' R U2 R'",
    "L' U' L U' L' U L U L F' L' F",
    "F R' F' R U R U' R'",
    "R U R' U R U' R' U' R' F R F'",
    "L F' L' U' L U F U' L'",
    "R' F R U R' U' F' U R",
    "R U R' U R U2 R' F R U R' U' F'",
    "R' U' R U' R' U2 R F R U R' U' F'",
    "F' U' L' U L F",
    "F U R U' R' F'",
    "F R U R' U' F'",
    "R' U' R' F R F' U R",
    "R' U' R' F R F' R' F R F' U R",
    "F R U R' U' R U R' U' F'",
    "r U' r2 U r2 U r2 U' r",
    "r' U r2 U' r2 U' r2 U r'",
    "F U R U' R' U R U' R' F'",
    "R U R' U R U' B U' B' R'",
    "l' U2 L U L' U' L U L' U l",
    "r U2 R' U' R U R' U' R U' r'",
    "R' F R U R U' R2 F' R2 U' R' U R U R'",
    "r' U' r U' R' U R U' R' U R r' U r",
    "R U R' U' M' U R U' r'",
  };

  size_t num_algos = sizeof(algos) / sizeof(char*);

  fprintf(stderr, "Generating OLL cases...\n");

  // LocDirCube *cases = malloc(216 * sizeof(LocDirCube));
  size_t *witnesses = malloc(216 * sizeof(size_t));
  Cube *replicas = malloc(216 * sizeof(Cube));
  size_t num_cases = 0;

  void generate(LocDirCube *ldc, Cube *replica, size_t depth) {
    Cube cube = to_cube(ldc);
    if (is_yellow_layer(&cube)) {
      size_t index = locdir_oll_index(ldc);
      for (size_t i = 0; i < num_cases; ++i) {
        if (index == witnesses[i]) {
          return;
        }
      }
      witnesses[num_cases] = index;
      replicas[num_cases] = *replica;
      // cases[num_cases] = *ldc;
      num_cases++;
    }
    if (depth <= 0) {
      return;
    }

    LocDirCube child = *ldc;
    locdir_U(&child);
    Cube child_replica = *replica;
    turn_U(&child_replica);
    generate(&child, &child_replica, depth - 1);

    child = *ldc;
    locdir_F(&child);
    locdir_R(&child);
    locdir_U(&child);
    locdir_R_prime(&child);
    locdir_U_prime(&child);
    locdir_F_prime(&child);
    child_replica = *replica;
    turn_F(&child_replica);
    turn_R(&child_replica);
    turn_U(&child_replica);
    turn_R_prime(&child_replica);
    turn_U_prime(&child_replica);
    turn_F_prime(&child_replica);
    generate(&child, &child_replica, depth - 1);
  }

  locdir_reset(&root);
  Cube root_replica;
  reset_oll(&root_replica);
  generate(&root, &root_replica, 140);

  fprintf(stderr, "%zu OLL cases generated\n", num_cases);

  Cube cube;
  Cube u_variant;
  Cube u2_variant;
  Cube u_prime_variant;
  LocDirCube ldc;
  sequence setup;
  size_t index;

  printf("<html>\n");
  printf("<head>\n");
  printf("<title>Shortest OLL algorithms (slice turn metric)</title>\n");
  printf("<style>\n");
  printf("table, th, td {border: 1px solid;}\n");
  printf("td {text-align:center; padding:0.5em;}\n");
  printf("table {border-collapse: collapse;}\n");
  printf("td.case {width:100px; height:100px; padding:0;}\n");
  printf("</style>\n");
  printf("</head>\n");
  printf("<body>\n");
  printf("<p>OLL (Orientation of the Last Layer) solves the top color of the last layer.</p>\n");
  printf("<p>Next step: <a href=\"pll.html\">PLL</a></p>\n");
  printf("<p>Shortest STM solutions discovered by <a href=\"https://github.com/frostburn/speedcube\">frostburn/speedcube</a>.</p>\n");
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
  printf("</tr>\n");

  size_t total = 0;

  for (size_t i = 0; i < num_algos; ++i) {
    fprintf(stderr, "Solving case %zu\n", i);
    setup = invert(parse(algos[i]));
    reset_oll(&cube);
    apply_sequence(&cube, setup);

    printf("<tr>\n");
    printf("<td>%zu</td>\n", i);
    printf("<td class=\"case\">\n");
    oll_svg(&cube);
    printf("</td>\n");

    locdir_reset(&ldc);
    locdir_apply_sequence(&ldc, setup);

    sequence solution = goalsphere_solve(&sphere, &ldc, search_depth);
    printf("<td>\n");
    print_sequence(solution);
    printf("</td>\n");
    printf("<td>%d</td>\n", sequence_length(solution));
    printf("<td>%d</td>\n", sequence_complexity(solution));
    printf("</tr>\n");

    index = locdir_oll_index(&ldc);
    for (size_t j = 0; j < num_cases; ++j) {
      if (witnesses[j] == index) {
        for (;j < num_cases - 1; ++j) {
          witnesses[j] = witnesses[j + 1];
        }
        num_cases--;
        break;
      }
    }
    // fprintf(stderr, "%zu = %zu @ %d\n", index, i, sequence_length(solution));

    total++;

    u_variant = cube;
    turn_U(&u_variant);
    if (!equals(&cube, &u_variant)) {
      printf("<tr>\n");
      printf("<td>%zu U</td>\n", i);
      printf("<td class=\"case\">\n");
      oll_svg(&u_variant);
      printf("</td>\n");

      locdir_reset(&ldc);
      locdir_apply_sequence(&ldc, setup);
      locdir_U(&ldc);
      sequence solution = goalsphere_solve(&sphere, &ldc, search_depth);
      printf("<td>\n");
      print_sequence(solution);
      printf("</td>\n");
      printf("<td>%d</td>\n", sequence_length(solution));
      printf("<td>%d</td>\n", sequence_complexity(solution));
      printf("</tr>\n");

      index = locdir_oll_index(&ldc);
      for (size_t j = 0; j < num_cases; ++j) {
        if (witnesses[j] == index) {
          for (;j < num_cases - 1; ++j) {
            witnesses[j] = witnesses[j + 1];
          }
          num_cases--;
          break;
        }
      }
      // fprintf(stderr, "%zu = %zu U @ %d\n", index, i, sequence_length(solution));
      total++;
    }
    u2_variant = cube;
    turn_U2(&u2_variant);
    if (!equals(&cube, &u2_variant) && !equals(&u_variant, &u2_variant)) {
      printf("<tr>\n");
      printf("<td>%zu U2</td>\n", i);
      printf("<td class=\"case\">\n");
      oll_svg(&u2_variant);
      printf("</td>\n");

      locdir_reset(&ldc);
      locdir_apply_sequence(&ldc, setup);
      locdir_U2(&ldc);
      sequence solution = goalsphere_solve(&sphere, &ldc, search_depth);
      printf("<td>\n");
      print_sequence(solution);
      printf("</td>\n");
      printf("<td>%d</td>\n", sequence_length(solution));
      printf("<td>%d</td>\n", sequence_complexity(solution));
      printf("</tr>\n");

      index = locdir_oll_index(&ldc);
      for (size_t j = 0; j < num_cases; ++j) {
        if (witnesses[j] == index) {
          for (;j < num_cases - 1; ++j) {
            witnesses[j] = witnesses[j + 1];
          }
          num_cases--;
          break;
        }
      }
      // fprintf(stderr, "%zu = %zu U2 @ %d\n", index, i, sequence_length(solution));
      total++;
    }
    u_prime_variant = cube;
    turn_U_prime(&u_prime_variant);
    if (!equals(&cube, &u_prime_variant) && !equals(&u_variant, &u_prime_variant) && !equals(&u_prime_variant, &u2_variant)) {
      printf("<tr>\n");
      printf("<td>%zu U'</td>\n", i);
      printf("<td class=\"case\">\n");
      oll_svg(&u_prime_variant);
      printf("</td>\n");

      locdir_reset(&ldc);
      locdir_apply_sequence(&ldc, setup);
      locdir_U_prime(&ldc);
      sequence solution = goalsphere_solve(&sphere, &ldc, search_depth);
      printf("<td>\n");
      print_sequence(solution);
      printf("</td>\n");
      printf("<td>%d</td>\n", sequence_length(solution));
      printf("<td>%d</td>\n", sequence_complexity(solution));
      printf("</tr>\n");

      index = locdir_oll_index(&ldc);
      for (size_t j = 0; j < num_cases; ++j) {
        if (witnesses[j] == index) {
          for (;j < num_cases - 1; ++j) {
            witnesses[j] = witnesses[j + 1];
          }
          num_cases--;
          break;
        }
      }
      // fprintf(stderr, "%zu = %zu U' @ %d\n", index, i, sequence_length(solution));
      total++;
    }
  }

  assert(num_cases == 0);

  printf("</table>\n");

  printf("<p>%zu algorithms in total.</p>\n", total);

  printf("</body>\n");
  printf("</html>\n");

  free_goalsphere(&sphere);

  return EXIT_SUCCESS;
}
