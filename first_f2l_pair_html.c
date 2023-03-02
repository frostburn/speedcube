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

int main() {
  FILE *fptr;
  size_t num_read;
  size_t tablebase_size;

  fprintf(stderr, "Loading tablebase for xcross.\n");
  Nibblebase tablebase = init_nibblebase(LOCDIR_XCROSS_INDEX_SPACE, &locdir_xcross_index);
  #if SCISSORS_ENABLED
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
  #if SCISSORS_ENABLED
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
  printf("<p>");
  printf("<span style=\"color:blue\">Blue</span> face is <b>F</b>ront.<br>");
  printf("<span style=\"color:red\">Red</span> face is <b>R</b>ight.<br>");
  printf("Gray face is <b>U</b>p.<br>");
  printf("<span style=\"color:lime\">Green</span> face is <b>B</b>ack.<br>");
  printf("<span style=\"color:orange\">Orange</span> face is <b>L</b>eft.<br>");
  printf("White face is <b>D</b>own.</p>");
  #if SCISSORS_ENABLED
  printf("<p>Scissor moves [in square brackets] can in principle be performed in one single motion.</p>");
  #endif

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
