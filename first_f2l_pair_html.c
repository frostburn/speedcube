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
  return "gray";
}

const double FR_HEIGHT = 50;
const double FL_HEIGHT = 45;

const double UFL_X = 5;
const double UFL_Y = 30;

const double UFR_X = 50;
const double UFR_Y = 45;

const double DFL_X = 7;
const double DFL_Y = UFL_Y + FL_HEIGHT;

const double DFR_X = 50;
const double DFR_Y = UFR_Y + FR_HEIGHT;

const double UBR_X = 100 - UFL_X;
const double UBR_Y = UFL_Y;

const double DBR_X = 100 - DFL_X;
const double DBR_Y = DFL_Y;

const double UBL_X = 50;
const double UBL_Y = 15;

double Fx(double i, double j) {
  double u = UFL_X + (UFR_X - UFL_X) * i / 3;
  double d = DFL_X + (DFR_X - DFL_X) * i / 3;
  return u + (d - u) * j / 3;
}

double Fy(double i, double j) {
  double l = UFL_Y + (DFL_Y - UFL_Y) * j / 3;
  double r = UFR_Y + (DFR_Y - UFR_Y) * j / 3;
  return l + (r - l) * i / 3;
}

double Rx(double i, double j) {
  double u = UFR_X + (UBR_X - UFR_X) * i / 3;
  double d = DFR_X + (DBR_X - DFR_X) * i / 3;
  return u + (d - u) * j / 3;
}

double Ry(double i, double j) {
  double l = UFR_Y + (DFR_Y - UFR_Y) * j / 3;
  double r = UBR_Y + (DBR_Y - UBR_Y) * j / 3;
  return l + (r - l) * i / 3;
}

double Ux(double i, double j) {
  double b = UBL_X + (UBR_X - UBL_X) * i / 3;
  double f = UFL_X + (UFR_X - UFL_X) * i / 3;
  return b + (f - b) * j / 3;
}

double Uy(double i, double j) {
  double l = UBL_Y + (UFL_Y - UBL_Y) * j / 3;
  double r = UBR_Y + (UFR_Y - UBR_Y) * j / 3;
  return l + (r - l) * i / 3;
}

void ufr_svg(LocDirCube *ldc) {
  Cube cube = to_cube(ldc);
  bitboard p;
  char *fill;
  printf("<svg viewBox=\"0 0 100 100\" width=\"100%%\" height=\"100%%\">\n");

  printf(" <g stroke=\"black\" stroke-width=\"1.1\" stroke-linejoin=\"bevel\">\n");
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      // F face
      p = 1ULL << (9 + i + 3*j);
      fill = sticker_fill(&cube, p);
      printf(
        "  <polygon points=\"%g,%g %g,%g %g,%g, %g,%g\" fill=\"%s\"/>\n",
        Fx(i+0, j+0), Fy(i+0, j+0),
        Fx(i+1, j+0), Fy(i+1, j+0),
        Fx(i+1, j+1), Fy(i+1, j+1),
        Fx(i+0, j+1), Fy(i+0, j+1),
        fill
      );

      // R face
      p = 1ULL << (18 + i + 3*j);
      fill = sticker_fill(&cube, p);

      printf(
        "  <polygon points=\"%g,%g %g,%g %g,%g, %g,%g\" fill=\"%s\"/>\n",
        Rx(i+0, j+0), Ry(i+0, j+0),
        Rx(i+1, j+0), Ry(i+1, j+0),
        Rx(i+1, j+1), Ry(i+1, j+1),
        Rx(i+0, j+1), Ry(i+0, j+1),
        fill
      );

      // U face
      p = 1ULL << (4*9 + i + 3*j);
      fill = sticker_fill(&cube, p);

      printf(
        "  <polygon points=\"%g,%g %g,%g %g,%g, %g,%g\" fill=\"%s\"/>\n",
        Ux(i+0, j+0), Uy(i+0, j+0),
        Ux(i+1, j+0), Uy(i+1, j+0),
        Ux(i+1, j+1), Uy(i+1, j+1),
        Ux(i+0, j+1), Uy(i+0, j+1),
        fill
      );
    }
  }
  printf(" </g>");
  printf("</svg>\n");
}

int main() {
  FILE *fptr;
  size_t num_read;
  size_t tablebase_size;

  fprintf(stderr, "Loading tablebase for xcross.\n");
  Nibblebase tablebase = init_nibblebase(LOCDIR_XCROSS_INDEX_SPACE, &locdir_xcross_index);
  fptr = fopen("./tables/xcross.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_XCROSS_INDEX_SPACE + 1)/2;
  num_read = fread(tablebase.octets, sizeof(unsigned char), tablebase_size, fptr);
  if (num_read != tablebase_size) {
    fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, tablebase_size);
    exit(EXIT_FAILURE);
  }
  fclose(fptr);

  fprintf(stderr, "Generating cases...\n");
  LocDirCube root;

  LocDirCube *cases = malloc(384 * sizeof(LocDirCube));
  size_t num_cases = 0;

  locdir_reset_xcross(&root);
  for (int i = 0; i < 6; ++i) {
    root.center_locs[i] = i;
  }
  for (int i = 0; i < 8; ++i) {
    root.corner_locs[4] = i;
    for (int j = 0; j < 3; ++j) {
      root.corner_dirs[4] = j;
      for (int k = 0; k < 8; ++k) {
        root.edge_locs[4] = k;
        for (int l = 0; l < 2; ++l) {
          root.edge_dirs[4] = l;
          cases[num_cases++] = root;
        }
      }
    }
  }

  fprintf(stderr, "%zu cases generated\n", num_cases);

  fprintf(stderr, "Solving cases...\n");
  sequence *solutions = malloc(384 * sizeof(sequence));
  size_t num_solutions;
  int max_length = 0;

  for (size_t i = 0; i < num_cases; ++i) {
    locdir_y2(cases + i);
    solutions[num_solutions++] = nibble_solve(&tablebase, cases + i, &is_better_semistable);
    int length = sequence_length(solutions[num_solutions - 1]);
    max_length = max_length > length ? max_length : length;
  }
  fprintf(stderr, "Priting results...\n");

  printf("<html>\n");
  printf("<head>\n");
  printf("<title>Shortest F2L algorithms (first pair, slice turn metric)</title>\n");
  printf("<style>\n");
  printf("table, th, td {border: 1px solid;}\n");
  printf("td {text-align:center; padding:0.5em;}\n");
  printf("table {border-collapse: collapse;}\n");
  printf("td.case {width:100px; height:100px; padding:0;}\n");
  printf("</style>\n");
  printf("</head>\n");
  printf("<body>\n");
  printf("<p>Shortest algorithms for solving the first F2L pair.</p>");
  printf("<p>");
  printf("<span style=\"color:blue\">Blue</span> face is <b>F</b>ront.<br>");
  printf("<span style=\"color:red\">Red</span> face is <b>R</b>ight.<br>");
  printf("Gray face is <b>U</b>p.<br>");
  printf("<span style=\"color:lime\">Green</span> face is <b>B</b>ack.<br>");
  printf("<span style=\"color:orange\">Orange</span> face is <b>L</b>eft.<br>");
  printf("White face is <b>D</b>own.</p>");

  for (int l = 2; l <= max_length; ++l) {
    int length = l;
    if (l == 2) {
      length = 0;
    }
    printf("<h2>%d moves</h2>\n", length);

    printf("<table>\n");
    printf("<tr>\n");
    printf("<th>Case</th>\n");
    printf("<th>Opposite view</th>\n");
    printf("<th>Algorithm</th>\n");
    printf("</tr>\n");

    for (size_t i = 0; i < num_cases; ++i) {
      if (sequence_length(solutions[i]) != length) {
        continue;
      }
      printf("<tr>\n");
      printf("<td class=\"case\">\n");
      cases[i].center_locs[4] = -1;
      ufr_svg(cases + i);
      printf("</td>\n");

      LocDirCube view = cases[i];
      locdir_x2(&view);
      locdir_y_prime(&view);
      printf("<td class=\"case\">\n");
      ufr_svg(&view);
      printf("</td>\n");

      printf("<td>\n");
      print_sequence(solutions[i]);
      printf("</td>\n");
      printf("</tr>\n");
    }

    printf("</table>\n");
  }

  printf("<p>%zu algorithms in total.</p>\n", num_cases);

  printf("</body>\n");
  printf("</html>\n");

  free(cases);

  return EXIT_SUCCESS;
}
