typedef struct {
  Nibblebase first;
  Nibblebase last;
  Nibblebase corners;
  IDAstar ida;
} Solver;

Solver GLOBAL_SOLVER;

unsigned char global_estimator(LocDirCube *ldc) {
  unsigned char first_depth = get_nibble(&GLOBAL_SOLVER.first, (*GLOBAL_SOLVER.first.index_func)(ldc));
  unsigned char last_depth = get_nibble(&GLOBAL_SOLVER.last, (*GLOBAL_SOLVER.last.index_func)(ldc));
  unsigned char corners_depth = get_nibble(&GLOBAL_SOLVER.corners, (*GLOBAL_SOLVER.corners.index_func)(ldc));

  if (first_depth > last_depth && first_depth > corners_depth) {
    return first_depth;
  }
  if (corners_depth > last_depth) {
    return corners_depth;
  }
  return last_depth;
}

void prepare_global_solver() {
  FILE *fptr;
  size_t num_read;
  size_t tablebase_size;

  fprintf(stderr, "Loading tablebase for first 7 edges.\n");
  GLOBAL_SOLVER.first = init_nibblebase(LOCDIR_FIRST_7_EDGE_INDEX_SPACE, &locdir_first_7_edge_index);
  fptr = fopen("./tables/first_7_edges.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_FIRST_7_EDGE_INDEX_SPACE + 1)/2;
  num_read = fread(GLOBAL_SOLVER.first.octets, sizeof(unsigned char), tablebase_size, fptr);
  if (num_read != tablebase_size) {
    fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, tablebase_size);
    exit(EXIT_FAILURE);
  }
  fclose(fptr);

  fprintf(stderr ,"Loading tablebase for last 7 edges.\n");
  GLOBAL_SOLVER.last = init_nibblebase(LOCDIR_LAST_7_EDGE_INDEX_SPACE, &locdir_last_7_edge_index);
  fptr = fopen("./tables/last_7_edges.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_LAST_7_EDGE_INDEX_SPACE + 1)/2;
  num_read = fread(GLOBAL_SOLVER.last.octets, sizeof(unsigned char), tablebase_size, fptr);
  if (num_read != tablebase_size) {
    fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, tablebase_size);
    exit(EXIT_FAILURE);
  }
  fclose(fptr);

  fprintf(stderr, "Loading tablebase for the corners.\n");
  GLOBAL_SOLVER.corners = init_nibblebase(LOCDIR_CORNER_INDEX_SPACE, &locdir_corner_index);
  fptr = fopen("./tables/corners.bin", "rb");
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  tablebase_size = (LOCDIR_CORNER_INDEX_SPACE + 1)/2;
  num_read = fread(GLOBAL_SOLVER.corners.octets, sizeof(unsigned char), tablebase_size, fptr);
  if (num_read != tablebase_size) {
    fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, tablebase_size);
    exit(EXIT_FAILURE);
  }
  fclose(fptr);

  GLOBAL_SOLVER.ida.is_solved = locdir_centerless_solved;
  GLOBAL_SOLVER.ida.estimator = global_estimator;
}

void free_global_solver() {
  free_nibblebase(&GLOBAL_SOLVER.first);
  free_nibblebase(&GLOBAL_SOLVER.last);
  free_nibblebase(&GLOBAL_SOLVER.corners);
}
