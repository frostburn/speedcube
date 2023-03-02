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
  radius = 6;
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

  Cube cubes[4];
  LocDirCube ldcs[4];
  sequence setup;
  size_t index;

  printf("<html>\n");
  printf("<head>\n");
  #if SCISSORS_ENABLED
  printf("<title>Shortest OLL algorithms (scissor turn metric)</title>\n");
  #else
  printf("<title>Shortest OLL algorithms (slice turn metric)</title>\n");
  #endif
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
  printf("</tr>\n");

  size_t total = 0;

  char *filename = malloc(256 * sizeof(char));

  for (size_t i = 0; i < num_algos; ++i) {
    fprintf(stderr, "Solving case %zu\n", i);
    setup = invert(parse(algos[i]));
    reset_oll(cubes);
    apply_sequence(cubes, setup);

    cubes[1] = cubes[0];
    cubes[2] = cubes[0];
    cubes[3] = cubes[0];
    turn_U(cubes + 1);
    turn_U2(cubes + 2);
    turn_U_prime(cubes + 3);

    locdir_reset(ldcs);
    locdir_apply_sequence(ldcs, setup);
    ldcs[1] = ldcs[0];
    ldcs[2] = ldcs[0];
    ldcs[3] = ldcs[0];
    locdir_U(ldcs + 1);
    locdir_U2(ldcs + 2);
    locdir_U_prime(ldcs + 3);

    for (size_t j = 0; j < 4; ++j) {
      bool seen = false;
      for (size_t k = 0; k < j; ++k) {
        if (equals(cubes + j, cubes + k)) {
          seen = true;
          break;
        }
      }
      if (seen) {
        continue;
      }

      index = locdir_oll_index(ldcs + j);
      for (size_t k = 0; k < num_cases; ++k) {
        if (witnesses[k] == index) {
          for (;k < num_cases - 1; ++k) {
            witnesses[k] = witnesses[k + 1];
          }
          num_cases--;
          break;
        }
      }
      // fprintf(stderr, "%zu = %zu @ %d\n", index, i, sequence_length(solution));

      sequence* solutions = goalsphere_solve_all(&sphere, ldcs + j, search_depth);
      if (solutions == NULL) {
        fprintf(stderr, "Failed to solve.\n");
        continue;
      }
      sequence solution = INVALID;
      sequence* candidate = solutions;
      while (*candidate != SENTINEL) {
        solution = is_better(solution, *candidate) ? solution : *candidate;
        candidate++;
      }

      printf("<tr>\n");
      if (j == 0) {
        printf("<td>%zu</td>\n", i);
      } else if (j == 1) {
        printf("<td>%zu U</td>\n", i);
      } else if (j == 2) {
        printf("<td>%zu U2</td>\n", i);
      } else {
        printf("<td>%zu U'</td>\n", i);
      }
      printf("<td class=\"case\">\n");
      oll_svg(cubes + j);
      printf("</td>\n");
      printf("<td>\n");
      #if SCISSORS_ENABLED
      printf("<a href=\"txt/oll_scissors_%zu_%zu.txt\">\n", i, j);
      #else
      printf("<a href=\"txt/oll_%zu_%zu.txt\">\n", i, j);
      #endif
      print_sequence(solution);
      printf("</a>\n");
      printf("</td>\n");
      printf("<td>%d</td>\n", sequence_length(solution));
      printf("<td>%d</td>\n", sequence_complexity(solution));
      printf("</tr>\n");

      #if SCISSORS_ENABLED
      sprintf(filename, "txt/oll_scissors_%zu_%zu.txt", i, j);
      #else
      sprintf(filename, "txt/oll_%zu_%zu.txt", i, j);
      #endif
      FILE *fptr = fopen(filename, "w");
      candidate = solutions;
      while (*candidate != SENTINEL) {
        fprint_sequence(fptr, *candidate);
        fprintf(fptr, "\n");
        candidate++;
      }
      fclose(fptr);

      free(solutions);

      total++;
    }
  }

  if (num_cases != 0) {
    fprintf(stderr, "%zu cases remain!\n", num_cases);
  }
  assert(num_cases == 0);

  printf("</table>\n");

  printf("<p>%zu algorithms in total.</p>\n", total);

  printf("</body>\n");
  printf("</html>\n");

  free_goalsphere(&sphere);
  free(filename);

  return EXIT_SUCCESS;
}
