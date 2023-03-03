#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"

#include "cube.c"
#include "moves.c"
#include "sequence.c"
#include "locdir.c"
#include "tablebase.c"
#include "hashset.c"
#include "goalsphere.c"
#include "ida_star.c"

// TODO: Scissors

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

  LocDirCube edges;
  locdir_reset_edges(&edges);

  printf("Loading database for the last 6 moves of an edges-only cube.\n");
  GoalSphere edge_sphere;
  edge_sphere.hash_func = locdir_edge_index;
  edge_sphere.goal_hash = locdir_edge_index(&edges);
  edge_sphere.num_sets = 6;
  edge_sphere.sets = malloc(edge_sphere.num_sets * sizeof(HashSet));
  edge_sphere.sets[0].magnitude = 5;
  edge_sphere.sets[0].num_elements = 27;
  edge_sphere.sets[1].magnitude = 10;
  edge_sphere.sets[1].num_elements = 501;
  edge_sphere.sets[2].magnitude = 14;
  edge_sphere.sets[2].num_elements = 9121;
  edge_sphere.sets[3].magnitude = 18;
  edge_sphere.sets[3].num_elements = 157886;
  edge_sphere.sets[4].magnitude = 22;
  edge_sphere.sets[4].num_elements = 2612316;
  edge_sphere.sets[5].magnitude = 26;
  edge_sphere.sets[5].num_elements = 41391832;
  fptr = fopen("./tables/edge_sphere.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < edge_sphere.num_sets; ++i) {
    size_t size = 1 << edge_sphere.sets[i].magnitude;
    edge_sphere.sets[i].mask = size - 1;
    edge_sphere.sets[i].empty_value = edge_sphere.goal_hash;
    edge_sphere.sets[i].elements = malloc(size * sizeof(size_t));
    num_read = fread(edge_sphere.sets[i].elements, sizeof(size_t), size, fptr);
    if (num_read != size) {
      fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, size);
      exit(EXIT_FAILURE);
    }
  }
  fclose(fptr);

  unsigned char sphere_depth = edge_sphere.num_sets;

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

    ida_star_solve(&ida, &edges, 0);

    printf("Found a solution in %zu moves:\n", ida.path_length - 1 + sphere_depth);

    sequence first_steps = ida_to_sequence(&ida);
    locdir_apply_sequence(&edges, first_steps);
    sequence final_steps = goalsphere_solve(&edge_sphere, &edges, 0, &is_better);
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

    ida_star_solve(&ida, &edges, 0);
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
