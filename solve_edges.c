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
#include "ida_star.c"

int main() {
  srand(time(NULL));

  FILE *fptr;
  size_t num_read;
  size_t tablebase_size;

  fprintf(stderr, "Loading tablebase for first 7 edges.\n");
  Nibblebase first = init_nibblebase(LOCDIR_FIRST_7_EDGE_INDEX_SPACE, &locdir_first_7_edge_index);
  fptr = fopen("./tables/first_7_edges.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_FIRST_7_EDGE_INDEX_SPACE + 1)/2;
  num_read = fread(first.octets, sizeof(unsigned char), tablebase_size, fptr);
  if (num_read != tablebase_size) {
    fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, tablebase_size);
    exit(EXIT_FAILURE);
  }
  fclose(fptr);

  fprintf(stderr ,"Loading tablebase for last 7 edges.\n");
  Nibblebase last = init_nibblebase(LOCDIR_LAST_7_EDGE_INDEX_SPACE, &locdir_last_7_edge_index);
  fptr = fopen("./tables/last_7_edges.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_LAST_7_EDGE_INDEX_SPACE + 1)/2;
  num_read = fread(last.octets, sizeof(unsigned char), tablebase_size, fptr);
  if (num_read != tablebase_size) {
    fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, tablebase_size);
    exit(EXIT_FAILURE);
  }
  fclose(fptr);

  printf("Loading database for the last 6 moves of an edges-only cube.\n");
  GoalSphere edge_sphere;
  edge_sphere.hash_func = locdir_edge_index;
  edge_sphere.num_sets = 6 + 1;
  edge_sphere.sets = malloc(edge_sphere.num_sets * sizeof(size_t*));
  edge_sphere.set_sizes = malloc(edge_sphere.num_sets * sizeof(size_t));
  edge_sphere.set_sizes[0] = 1;
  edge_sphere.set_sizes[1] = 27;
  edge_sphere.set_sizes[2] = 501;
  edge_sphere.set_sizes[3] = 9121;
  edge_sphere.set_sizes[4] = 157886;
  edge_sphere.set_sizes[5] = 2612316;
  edge_sphere.set_sizes[6] = 41391832;
  fptr = fopen("./tables/edge_sphere.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < edge_sphere.num_sets; ++i) {
    edge_sphere.sets[i] = malloc(edge_sphere.set_sizes[i] * sizeof(size_t));
    num_read = fread(edge_sphere.sets[i], sizeof(size_t), edge_sphere.set_sizes[i], fptr);
    if (num_read != edge_sphere.set_sizes[i]) {
      fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, edge_sphere.set_sizes[i]);
      exit(EXIT_FAILURE);
    }
  }
  fclose(fptr);

  unsigned char sphere_depth = edge_sphere.num_sets - 1;

  unsigned char estimator(LocDirCube *ldc) {
    unsigned char depth = get_nibble(&first, (*first.index_func)(ldc));
    unsigned char last_depth = get_nibble(&last, (*last.index_func)(ldc));

    if (last_depth > depth) {
      depth = last_depth;
    }

    if (depth > sphere_depth) {
      return depth - sphere_depth;
    }
    return 0;
  }

  bool is_solved(LocDirCube *ldc) {
    return goalsphere_shell(&edge_sphere, ldc);
  }

  IDAstar ida;
  ida.estimator = estimator;
  ida.is_solved = is_solved;

  Cube cube;
  LocDirCube edges;
  locdir_reset_edges(&edges);
  for (char i = 0; i < 6; ++i) {
    edges.center_locs[i] = i;
  }

  printf("Solving a few scrambles...\n");
  for (size_t j = 0; j < 5; j++) {
    printf("Scramble:\n");
    locdir_realign(&edges);
    locdir_scramble(&edges);
    cube = to_cube(&edges);
    render(&cube);

    if (goalsphere_depth(&edge_sphere, &edges, 0) <= sphere_depth) {
      printf("Too easy.\n");
      continue;
    }

    ida_star_solve(&ida, &edges);

    printf("Found a solution in %zu moves:\n", ida.path_length - 1 + sphere_depth);

    sequence first_steps = ida_to_sequence(&ida);
    locdir_apply_sequence(&edges, first_steps);
    sequence final_steps = goalsphere_solve(&edge_sphere, &edges, 0);
    locdir_apply_sequence(&edges, final_steps);
    print_sequence(concat(first_steps, final_steps));
    cube = to_cube(&edges);
    render(&cube);
    printf("\n");
  }

  printf("Collecting statistics...\n");

  clock_t start = clock();
  size_t total_solves = 10000;
  size_t total_moves = 0;
  size_t min_moves = ~0ULL;
  size_t max_moves = 0;
  for (size_t i = 0; i < total_solves; ++i) {
    locdir_reset_edges(&edges);
    locdir_scramble(&edges);

    if (goalsphere_depth(&edge_sphere, &edges, 0) <= sphere_depth) {
      min_moves = 0;
      continue;
    }

    ida_star_solve(&ida, &edges);
    size_t num_moves = ida.path_length - 1;
    total_moves += num_moves;
    if (num_moves < min_moves) {
      min_moves = num_moves;
    }
    if (num_moves > max_moves) {
      max_moves = num_moves;
    }
  }
  clock_t end = clock();

  double took = end - start;
  took /= CLOCKS_PER_SEC;

  printf("Solved %zu scrambles in %g seconds (%g ms / solution).\n", total_solves, took, 1000 * took/total_solves);
  printf("Minimum number of moves in a solution = %zu\n", min_moves + sphere_depth);
  printf("Average number of moves in a solution = %g\n", ((double)(total_moves + sphere_depth * total_solves)) / total_solves);
  printf("Maximum number of moves in a solution = %zu\n", max_moves + sphere_depth);

  free_nibblebase(&first);
  free_nibblebase(&last);
  free_goalsphere(&edge_sphere);

  return EXIT_SUCCESS;
}
