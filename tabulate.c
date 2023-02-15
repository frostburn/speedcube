#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"

#include "cube.c"
#include "moves.c"
#include "sequence.c"
#include "bst.c"
#include "indexer.c"
#include "locdir.c"
#include "tablebase.c"
#include "goalsphere.c"

int main() {
  Cube cube;
  LocDirCube ldc;
  FILE *fptr;
  size_t tablebase_size;
  Nibblebase tablebase;
  GoalSphere sphere;

  printf("Creating tablebase for corners only...\n");
  tablebase = init_nibblebase(LOCDIR_CORNER_INDEX_SPACE, &locdir_corner_index);
  printf("Populating corners-only tablebase...\n");
  locdir_reset_corners(&ldc);
  cube = to_cube(&ldc);
  render(&cube);
  populate_nibblebase(&tablebase, &ldc);
  printf("Storing result...\n");
  fptr = fopen("./tables/corners.bin", "wb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open storage.\n");
    return EXIT_FAILURE;
  }
  tablebase_size = (LOCDIR_CORNER_INDEX_SPACE + 1)/2;
  fwrite(tablebase.octets, sizeof(unsigned char), tablebase_size, fptr);
  fclose(fptr);
  free_nibblebase(&tablebase);

  printf("Corners done.\n");

  printf("\n");

  printf("Creating tablebase for the first 7 edge cubies...\n");
  tablebase = init_nibblebase(LOCDIR_FIRST_7_EDGE_INDEX_SPACE, &locdir_first_7_edge_index);
  printf("Populating first 7 edges tablebase...\n");
  locdir_reset_edges(&ldc);
  for (size_t i = 7; i < 12; ++i) {
    ldc.edge_locs[i] = -1;
  }
  cube = to_cube(&ldc);
  render(&cube);
  populate_nibblebase(&tablebase, &ldc);
  printf("Storing result...\n");
  fptr = fopen("./tables/first_7_edges.bin", "wb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open storage.\n");
    return EXIT_FAILURE;
  }
  tablebase_size = (LOCDIR_FIRST_7_EDGE_INDEX_SPACE + 1)/2;
  fwrite(tablebase.octets, sizeof(unsigned char), tablebase_size, fptr);
  fclose(fptr);
  free_nibblebase(&tablebase);

  printf("Creating tablebase for the last 7 edge cubies...\n");
  tablebase = init_nibblebase(LOCDIR_LAST_7_EDGE_INDEX_SPACE, &locdir_last_7_edge_index);
  printf("Populating last 7 edges tablebase...\n");
  locdir_reset_edges(&ldc);
  for (size_t i = 7; i < 12; ++i) {
    ldc.edge_locs[11 - i] = -1;
  }
  cube = to_cube(&ldc);
  render(&cube);
  populate_nibblebase(&tablebase, &ldc);
  printf("Storing result...\n");
  fptr = fopen("./tables/last_7_edges.bin", "wb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open storage.\n");
    return EXIT_FAILURE;
  }
  tablebase_size = (LOCDIR_LAST_7_EDGE_INDEX_SPACE + 1)/2;
  fwrite(tablebase.octets, sizeof(unsigned char), tablebase_size, fptr);
  fclose(fptr);
  free_nibblebase(&tablebase);

  printf("Creating a goal sphere around solved edges...\n");
  locdir_reset_edges(&ldc);
  cube = to_cube(&ldc);
  render(&cube);
  sphere = init_goalsphere(&ldc, 6, &locdir_edge_index);
  printf("Storing result...\n");
  fptr = fopen("./tables/edge_sphere.bin", "wb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open storage.\n");
    return EXIT_FAILURE;
  }
  for (size_t i = 0; i < sphere.num_sets; ++i) {
    printf("Depth %zu has %zu unique configurations.\n", i, sphere.set_sizes[i]);
    fwrite(sphere.sets[i], sizeof(size_t), sphere.set_sizes[i], fptr);
  }
  // Depth 0 has 1 unique configurations.
  // Depth 1 has 27 unique configurations.
  // Depth 2 has 501 unique configurations.
  // Depth 3 has 9121 unique configurations.
  // Depth 4 has 157886 unique configurations.
  // Depth 5 has 2612316 unique configurations.
  // Depth 6 has 41391832 unique configurations.
  fclose(fptr);
  free_goalsphere(&sphere);

  printf("Edges done.\n");

  printf("\n");

  return EXIT_SUCCESS;
}
