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
#include "svg.c"

typedef struct {
  LocDirCube ldc;
  sequence solution;
} Case;

// Bottom omitted for brevity
const bitboard SLOT_LB = (288ULL << 27) | 72;
const bitboard SLOT_LF = 288 | (72 << 9);
const bitboard SLOT_RF = (288 << 9) | (72 << 18);
const bitboard SLOT_RB = (288 << 18) | (72ULL << 27);

Cube original;

bool slot_modified(Cube *cube, bitboard slot) {
  return (
    ((cube->a & slot) != (original.a & slot)) ||
    ((cube->b & slot) != (original.b & slot)) ||
    ((cube->c & slot) != (original.c & slot))
  );
}

int cmp_case(const void *a, const void *b) {
  Cube x;
  Cube y;

  reset(&x);
  reset(&y);

  apply_sequence(&x, ((Case*)a)->solution);
  apply_sequence(&y, ((Case*)b)->solution);

  int x_lb = slot_modified(&x, SLOT_LB);
  int x_lf = slot_modified(&x, SLOT_LF);
  int x_rb = slot_modified(&x, SLOT_RB);

  int y_lb = slot_modified(&y, SLOT_LB);
  int y_lf = slot_modified(&y, SLOT_LF);
  int y_rb = slot_modified(&y, SLOT_RB);

  if (x_lb + x_lf + x_rb < y_lb + y_lf + y_rb) {
    return -1;
  }
  if (x_lb + x_lf + x_rb > y_lb + y_lf + y_rb) {
    return 1;
  }

  x_lb += 2*x_lf + 4*x_rb;
  y_lb += 2*y_lf + 4*y_rb;

  return x_lb - y_lb;
}

int main() {
  reset(&original);

  FILE *fptr;
  size_t num_read;
  size_t tablebase_size;

  fprintf(stderr, "Loading tablebase for xcross.\n");
  Nibblebase tablebase = init_nibblebase(LOCDIR_XCROSS_INDEX_SPACE, &locdir_xcross_index);
  #ifdef SCISSORS_ENABLED
  fptr = fopen("./tables/xcross_scissors.bin", "rb");
  #else
  fptr = fopen("./tables/xcross.bin", "rb");
  #endif
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

  Case *cases = malloc(384 * sizeof(Case));
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
          cases[num_cases++].ldc = root;
        }
      }
    }
  }

  fprintf(stderr, "%zu cases generated\n", num_cases);

  fprintf(stderr, "Solving cases...\n");
  int max_length = 0;

  for (size_t i = 0; i < num_cases; ++i) {
    locdir_y2(&cases[i].ldc);
    cases[i].solution = nibble_solve(&tablebase, &cases[i].ldc, &is_better_stable);
    cases[i].ldc.center_locs[4] = -1;
    int length = sequence_length(cases[i].solution);
    max_length = max_length > length ? max_length : length;
  }

  qsort(cases, 384, sizeof(Case), cmp_case);

  fprintf(stderr, "Priting results...\n");

  printf("<html>\n");
  printf("<head>\n");
  #ifdef SCISSORS_ENABLED
  printf("<title>Shortest F2L algorithms (first pair, scissor turn metric)</title>\n");
  #else
  printf("<title>Shortest F2L algorithms (first pair, slice turn metric)</title>\n");
  #endif
  printf("<style>\n");
  printf("table, th, td {border: 1px solid;}\n");
  printf("td {text-align:center; padding:0.5em;}\n");
  printf("table {border-collapse: collapse;}\n");
  printf("td.case {width:100px; height:100px; padding:0;}\n");
  printf("</style>\n");
  printf("</head>\n");
  printf("<body>\n");
  printf("<p>Shortest algorithms for solving the first F2L pair.</p>");
  #ifdef SCISSORS_ENABLED
  printf("<p>There are also a list for the <a href=\"last-f2l-pair-scissors.html\">last pair</a>.</p>");
  #else
  printf("<p>There are also a list for the <a href=\"last-f2l-pair.html\">last pair</a>.</p>");
  #endif
  printf("<p>");
  printf("<span style=\"color:blue\">Blue</span> face is <b>F</b>ront.<br>");
  printf("<span style=\"color:red\">Red</span> face is <b>R</b>ight.<br>");
  printf("Gray face is <b>U</b>p.<br>");
  printf("<span style=\"color:lime\">Green</span> face is <b>B</b>ack.<br>");
  printf("<span style=\"color:orange\">Orange</span> face is <b>L</b>eft.<br>");
  printf("White face is <b>D</b>own.</p>");
  #ifdef SCISSORS_ENABLED
  printf("<p>Scissor moves [in square brackets] can in principle be performed in one single motion.</p>");
  #endif

  char *filename = malloc(256 * sizeof(char));

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
    printf("<th>Slots affected</th>\n");
    printf("</tr>\n");

    for (size_t i = 0; i < num_cases; ++i) {
      if (sequence_length(cases[i].solution) != length) {
        continue;
      }
      printf("<tr>\n");
      printf("<td class=\"case\">\n");
      ufr_svg(&cases[i].ldc);
      printf("</td>\n");

      LocDirCube view = cases[i].ldc;
      locdir_x2(&view);
      locdir_y_prime(&view);
      printf("<td class=\"case\">\n");
      ufr_svg(&view);
      printf("</td>\n");

      printf("<td>\n");
      #ifdef SCISSORS_ENABLED
      sprintf(filename, "txt/first_f2l_pair_scissors_%zu.txt", i);
      #else
      sprintf(filename, "txt/first_f2l_pair_%zu.txt", i);
      #endif
      printf("<a href=\"%s\">", filename);
      print_sequence(cases[i].solution);
      printf("</a>");
      printf("</td>\n");

      printf("<td>\n");
      Cube cube;
      reset(&cube);
      apply_sequence(&cube, cases[i].solution);
      bool comma = false;
      if (slot_modified(&cube, SLOT_LB)) {
        printf("LB");
        comma = true;
      }
      if (slot_modified(&cube, SLOT_LF)) {
        if (comma) {
          printf(", ");
        }
        printf("LF");
        comma = true;
      }
      if (slot_modified(&cube, SLOT_RB)) {
        if (comma) {
          printf(", ");
        }
        printf("RB");
      }
      if (length > 0) {
        assert(slot_modified(&cube, SLOT_RF));
      }
      printf("</td>\n");

      printf("</tr>\n");

      collection solutions = nibble_solve_all(&tablebase, &cases[i].ldc);
      FILE *fptr = fopen(filename, "w");
      collection it = solutions;
      while (*it != SENTINEL) {
        fprint_sequence(fptr, *it);
        fprintf(fptr, "\n");
        it++;
      }
      fclose(fptr);

      free(solutions);
    }

    printf("</table>\n");
  }

  printf("<p>%zu algorithms in total.</p>\n", num_cases);

  printf("</body>\n");
  printf("</html>\n");

  free(filename);
  free(cases);

  return EXIT_SUCCESS;
}
