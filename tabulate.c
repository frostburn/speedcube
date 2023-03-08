#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"

#include "cube.c"
#include "moves.c"
#include "sequence.c"
#include "locdir.c"
#include "tablebase.c"
#include "goalsphere.c"

void create_corner_tablebase() {
  Cube cube;
  LocDirCube ldc;
  FILE *fptr;
  size_t tablebase_size;
  Nibblebase tablebase;

  printf("Creating tablebase for corners only...\n");
  tablebase = init_nibblebase(LOCDIR_CORNER_INDEX_SPACE, &locdir_corner_index);
  printf("Populating corners-only tablebase...\n");
  locdir_reset_corners(&ldc);
  cube = to_cube(&ldc);
  render(&cube);
  populate_nibblebase(&tablebase, &ldc);
  printf("Storing result...\n");
  #ifdef SCISSORS_ENABLED
  fptr = fopen("./tables/corners_scissors.bin", "wb");
  #else
  fptr = fopen("./tables/corners.bin", "wb");
  #endif
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open storage.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_CORNER_INDEX_SPACE + 1)/2;
  fwrite(tablebase.octets, sizeof(unsigned char), tablebase_size, fptr);
  fclose(fptr);
  free_nibblebase(&tablebase);
}

void create_first_edges_tablebase() {
  Cube cube;
  LocDirCube ldc;
  FILE *fptr;
  size_t tablebase_size;
  Nibblebase tablebase;

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
  #ifdef SCISSORS_ENABLED
  fptr = fopen("./tables/first_7_edges_scissors.bin", "wb");
  #else
  fptr = fopen("./tables/first_7_edges.bin", "wb");
  #endif
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open storage.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_FIRST_7_EDGE_INDEX_SPACE + 1)/2;
  fwrite(tablebase.octets, sizeof(unsigned char), tablebase_size, fptr);
  fclose(fptr);
  free_nibblebase(&tablebase);
}

void create_last_edges_tablebase() {
  Cube cube;
  LocDirCube ldc;
  FILE *fptr;
  size_t tablebase_size;
  Nibblebase tablebase;

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
  #ifdef SCISSORS_ENABLED
  fptr = fopen("./tables/last_7_edges_scissors.bin", "wb");
  #else
  fptr = fopen("./tables/last_7_edges.bin", "wb");
  #endif
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open storage.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_LAST_7_EDGE_INDEX_SPACE + 1)/2;
  fwrite(tablebase.octets, sizeof(unsigned char), tablebase_size, fptr);
  fclose(fptr);
  free_nibblebase(&tablebase);
}

void create_xcross_tablebase() {
  Cube cube;
  LocDirCube ldc;
  FILE *fptr;
  size_t tablebase_size;
  Nibblebase tablebase;

  printf("Creating tablebase for xcross...\n");
  tablebase = init_nibblebase(LOCDIR_XCROSS_INDEX_SPACE, &locdir_xcross_index);
  printf("Populating xcross tablebase...\n");
  locdir_reset_xcross(&ldc);
  cube = to_cube(&ldc);
  render(&cube);
  populate_nibblebase(&tablebase, &ldc);
  printf("Storing result...\n");
  #ifdef SCISSORS_ENABLED
  fptr = fopen("./tables/xcross_scissors.bin", "wb");
  #else
  fptr = fopen("./tables/xcross.bin", "wb");
  #endif
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open storage.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_XCROSS_INDEX_SPACE + 1)/2;
  fwrite(tablebase.octets, sizeof(unsigned char), tablebase_size, fptr);
  fclose(fptr);
  free_nibblebase(&tablebase);
}

void create_edge_sphere() {
  Cube cube;
  LocDirCube ldc;
  FILE *fptr;
  GoalSphere sphere;

  printf("Creating a goal sphere around solved edges...\n");
  locdir_reset_edges(&ldc);
  cube = to_cube(&ldc);
  render(&cube);
  sphere = init_goalsphere(&ldc, 6, &locdir_edge_index);
  printf("Storing result...\n");
  #ifdef SCISSORS_ENABLED
  fptr = fopen("./tables/edge_sphere_scissors.bin", "wb");
  #else
  fptr = fopen("./tables/edge_sphere.bin", "wb");
  #endif
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open storage.\n");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < sphere.num_sets; ++i) {
    printf("Depth %zu has %zu unique configurations.\n", i, sphere.set_sizes[i]);
    fwrite(sphere.sets[i], sizeof(size_t), sphere.set_sizes[i], fptr);
  }
  // #ifdef SCISSORS_ENABLED
  // Depth 0 has 1 unique configurations.
  // Depth 1 has 45 unique configurations.
  // Depth 2 has 1347 unique configurations.
  // Depth 3 has 39471 unique configurations.
  // Depth 4 has 1090256 unique configurations.
  // Depth 5 has 28178805 unique configurations.
  // Depth 6 has 690100863 unique configurations.
  // #else
  // Depth 0 has 1 unique configurations.
  // Depth 1 has 27 unique configurations.
  // Depth 2 has 501 unique configurations.
  // Depth 3 has 9121 unique configurations.
  // Depth 4 has 157886 unique configurations.
  // Depth 5 has 2612316 unique configurations.
  // Depth 6 has 41391832 unique configurations.
  fclose(fptr);
  free_goalsphere(&sphere);
}

void create_3x3x3_sphere() {
  Cube cube;
  LocDirCube ldc;
  FILE *fptr;
  GoalSphere sphere;

  printf("Creating a goal sphere around the 3x3x3 solution (implicit centers)...\n");
  locdir_reset(&ldc);
  cube = to_cube(&ldc);
  render(&cube);
  sphere = init_goalsphere(&ldc, 6, &locdir_centerless_hash);
  printf("Storing result...\n");
  #ifdef SCISSORS_ENABLED
  fptr = fopen("./tables/centerless_sphere_scissors.bin", "wb");
  #else
  fptr = fopen("./tables/centerless_sphere.bin", "wb");
  #endif
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open storage.\n");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < sphere.num_sets; ++i) {
    printf("Depth %zu has %zu unique configurations.\n", i, sphere.set_sizes[i]);
    fwrite(sphere.sets[i], sizeof(size_t), sphere.set_sizes[i], fptr);
  }
  // #ifdef SCISSORS_ENABLED
  // Depth 0 has 1 unique configurations.
  // Depth 1 has 45 unique configurations.
  // Depth 2 has 1347 unique configurations.
  // Depth 3 has 39631 unique configurations.
  // Depth 4 has 1152290 unique configurations.
  // Depth 5 has 32717804 unique configurations.
  // Depth 6 has 917301225 unique configurations.
  // #else
  // Depth 0 has 1 unique configurations.
  // Depth 1 has 27 unique configurations.
  // Depth 2 has 501 unique configurations.
  // Depth 3 has 9175 unique configurations.
  // Depth 4 has 164900 unique configurations.
  // Depth 5 has 2912447 unique configurations.
  // Depth 6 has 50839041 unique configurations.
  fclose(fptr);
  free_goalsphere(&sphere);
}

void create_oll_sphere() {
  Cube cube;
  LocDirCube ldc;
  FILE *fptr;
  GoalSphere sphere;

  printf("Creating a OLL goal sphere around the 3x3x3 solution (implicit centers)...\n");
  locdir_reset(&ldc);
  cube = to_cube(&ldc);
  reset_oll(&cube);
  render(&cube);
  sphere = init_goalsphere(&ldc, 6, &locdir_oll_index);
  printf("Storing result...\n");
  #ifdef SCISSORS_ENABLED
  fptr = fopen("./tables/oll_sphere_scissors.bin", "wb");
  #else
  fptr = fopen("./tables/oll_sphere.bin", "wb");
  #endif
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open storage.\n");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < sphere.num_sets; ++i) {
    printf("Depth %zu has %zu unique configurations.\n", i, sphere.set_sizes[i]);
    fwrite(sphere.sets[i], sizeof(size_t), sphere.set_sizes[i], fptr);
  }
  // Depth 0 has 1 unique configurations.
  // Depth 1 has 21 unique configurations.
  // Depth 2 has 387 unique configurations.
  // Depth 3 has 7077 unique configurations.
  // Depth 4 has 126006 unique configurations.
  // Depth 5 has 2210527 unique configurations.
  // Depth 6 has 38327451 unique configurations.
  fclose(fptr);
  free_goalsphere(&sphere);
}

int main() {
  #ifdef SCISSORS_ENABLED
    printf("Scissor moves enabled.\n");
    #ifdef ALTERNATIVE_SCISSORS
      printf("Using alternative scissor implementations.\n");
    #endif
  #endif

  create_xcross_tablebase();

  create_corner_tablebase();

  create_first_edges_tablebase();
  create_last_edges_tablebase();

  create_edge_sphere();

  create_3x3x3_sphere();

  // create_oll_sphere();

  return EXIT_SUCCESS;
}
