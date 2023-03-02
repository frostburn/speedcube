typedef struct {
  Nibblebase first;
  Nibblebase last;
  Nibblebase corners;
  GoalSphere goal;
  GoalSphere edge_goal;
  IDAstar ida;
  IDAstar edge_ida;
} Solver;

Solver GLOBAL_SOLVER;

unsigned char global_estimator(LocDirCube *ldc) {
  unsigned char depth = get_nibble(&GLOBAL_SOLVER.first, (*GLOBAL_SOLVER.first.index_func)(ldc));
  unsigned char last_depth = get_nibble(&GLOBAL_SOLVER.last, (*GLOBAL_SOLVER.last.index_func)(ldc));
  unsigned char corners_depth = get_nibble(&GLOBAL_SOLVER.corners, (*GLOBAL_SOLVER.corners.index_func)(ldc));

  if (last_depth > depth) {
    depth = last_depth;
  }
  if (corners_depth > depth) {
    depth = corners_depth;
  }

  unsigned char goal_depth = GLOBAL_SOLVER.goal.num_sets - 1;

  if (depth < goal_depth) {
    return 0;
  }
  return depth - goal_depth;
}

unsigned char global_edge_estimator(LocDirCube *ldc) {
  unsigned char depth = get_nibble(&GLOBAL_SOLVER.first, (*GLOBAL_SOLVER.first.index_func)(ldc));
  unsigned char last_depth = get_nibble(&GLOBAL_SOLVER.last, (*GLOBAL_SOLVER.last.index_func)(ldc));

  if (last_depth > depth) {
    depth = last_depth;
  }

  unsigned char goal_depth = GLOBAL_SOLVER.edge_goal.num_sets - 1;

  if (depth < goal_depth) {
    return 0;
  }
  return depth - goal_depth;
}

bool global_is_solved(LocDirCube *ldc) {
  return goalsphere_shell(&GLOBAL_SOLVER.goal, ldc);
}

bool global_edge_is_solved(LocDirCube *ldc) {
  return goalsphere_shell(&GLOBAL_SOLVER.edge_goal, ldc);
}

void prepare_global_solver() {
  FILE *fptr;
  size_t num_read;
  size_t tablebase_size;

  fprintf(stderr, "Loading tablebase for first 7 edges.\n");
  GLOBAL_SOLVER.first = init_nibblebase(LOCDIR_FIRST_7_EDGE_INDEX_SPACE, &locdir_first_7_edge_index);
  #if SCISSORS_ENABLED
  fptr = fopen("./tables/first_7_edges_scissors.bin", "rb");
  #else
  fptr = fopen("./tables/first_7_edges.bin", "rb");
  #endif
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
  #if SCISSORS_ENABLED
  fptr = fopen("./tables/last_7_edges_scissors.bin", "rb");
  #else
  fptr = fopen("./tables/last_7_edges.bin", "rb");
  #endif
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
  #if SCISSORS_ENABLED
  fptr = fopen("./tables/corners_scissors.bin", "rb");
  #else
  fptr = fopen("./tables/corners.bin", "rb");
  #endif
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

  fprintf(stderr, "Loading database for the last 6 moves.\n");
  GLOBAL_SOLVER.goal.hash_func = locdir_centerless_hash;
  GLOBAL_SOLVER.goal.num_sets = 6 + 1;
  GLOBAL_SOLVER.goal.sets = malloc(GLOBAL_SOLVER.goal.num_sets * sizeof(size_t*));
  GLOBAL_SOLVER.goal.set_sizes = malloc(GLOBAL_SOLVER.goal.num_sets * sizeof(size_t));
  GLOBAL_SOLVER.goal.set_sizes[0] = 1;
  #if SCISSORS_ENABLED
  GLOBAL_SOLVER.goal.set_sizes[1] = 33;
  GLOBAL_SOLVER.goal.set_sizes[2] = 735;
  GLOBAL_SOLVER.goal.set_sizes[3] = 16183;
  GLOBAL_SOLVER.goal.set_sizes[4] = 348926;
  GLOBAL_SOLVER.goal.set_sizes[5] = 7268624;
  GLOBAL_SOLVER.goal.set_sizes[6] = 148062628;
  fptr = fopen("./tables/centerless_sphere_scissors.bin", "rb");
  #else
  GLOBAL_SOLVER.goal.set_sizes[1] = 27;
  GLOBAL_SOLVER.goal.set_sizes[2] = 501;
  GLOBAL_SOLVER.goal.set_sizes[3] = 9175;
  GLOBAL_SOLVER.goal.set_sizes[4] = 164900;
  GLOBAL_SOLVER.goal.set_sizes[5] = 2912447;
  GLOBAL_SOLVER.goal.set_sizes[6] = 50839041;
  fptr = fopen("./tables/centerless_sphere.bin", "rb");
  #endif
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < GLOBAL_SOLVER.goal.num_sets; ++i) {
    GLOBAL_SOLVER.goal.sets[i] = malloc(GLOBAL_SOLVER.goal.set_sizes[i] * sizeof(size_t));
    size_t num_read = fread(GLOBAL_SOLVER.goal.sets[i], sizeof(size_t), GLOBAL_SOLVER.goal.set_sizes[i], fptr);
    if (num_read != GLOBAL_SOLVER.goal.set_sizes[i]) {
      fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, GLOBAL_SOLVER.goal.set_sizes[i]);
      exit(EXIT_FAILURE);
    }
  }
  fclose(fptr);

  fprintf(stderr, "Loading database for the last 6 moves of an edges-only cube.\n");
  GLOBAL_SOLVER.edge_goal.hash_func = locdir_edge_index;
  GLOBAL_SOLVER.edge_goal.num_sets = 6 + 1;
  GLOBAL_SOLVER.edge_goal.sets = malloc(GLOBAL_SOLVER.edge_goal.num_sets * sizeof(size_t*));
  GLOBAL_SOLVER.edge_goal.set_sizes = malloc(GLOBAL_SOLVER.edge_goal.num_sets * sizeof(size_t));
  GLOBAL_SOLVER.edge_goal.set_sizes[0] = 1;
  #if SCISSORS_ENABLED
  GLOBAL_SOLVER.edge_goal.set_sizes[1] = 33;
  GLOBAL_SOLVER.edge_goal.set_sizes[2] = 735;
  GLOBAL_SOLVER.edge_goal.set_sizes[3] = 16059;
  GLOBAL_SOLVER.edge_goal.set_sizes[4] = 327692;
  GLOBAL_SOLVER.edge_goal.set_sizes[5] = 6093513;
  GLOBAL_SOLVER.edge_goal.set_sizes[6] = 107477715;
  fptr = fopen("./tables/edge_sphere_scissors.bin", "rb");
  #else
  GLOBAL_SOLVER.edge_goal.set_sizes[1] = 27;
  GLOBAL_SOLVER.edge_goal.set_sizes[2] = 501;
  GLOBAL_SOLVER.edge_goal.set_sizes[3] = 9121;
  GLOBAL_SOLVER.edge_goal.set_sizes[4] = 157886;
  GLOBAL_SOLVER.edge_goal.set_sizes[5] = 2612316;
  GLOBAL_SOLVER.edge_goal.set_sizes[6] = 41391832;
  fptr = fopen("./tables/edge_sphere.bin", "rb");
  #endif
  if (fptr == NULL) {
    fprintf(stderr, "Failed to open file.\n");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < GLOBAL_SOLVER.edge_goal.num_sets; ++i) {
    GLOBAL_SOLVER.edge_goal.sets[i] = malloc(GLOBAL_SOLVER.edge_goal.set_sizes[i] * sizeof(size_t));
    num_read = fread(GLOBAL_SOLVER.edge_goal.sets[i], sizeof(size_t), GLOBAL_SOLVER.edge_goal.set_sizes[i], fptr);
    if (num_read != GLOBAL_SOLVER.edge_goal.set_sizes[i]) {
      fprintf(stderr, "Failed to load data. Only %zu of %zu read.\n", num_read, GLOBAL_SOLVER.edge_goal.set_sizes[i]);
      exit(EXIT_FAILURE);
    }
  }
  fclose(fptr);

  GLOBAL_SOLVER.ida.is_solved = global_is_solved;
  GLOBAL_SOLVER.ida.estimator = global_estimator;

  GLOBAL_SOLVER.edge_ida.is_solved = global_edge_is_solved;
  GLOBAL_SOLVER.edge_ida.estimator = global_edge_estimator;

  #ifdef _OPENMP
  fprintf(stderr, "Parallel search enabled.\n");
  #endif
}

sequence global_solve(LocDirCube *ldc) {
  unsigned char lower_bound = goalsphere_depth(&GLOBAL_SOLVER.edge_goal, ldc, 0);
  if (lower_bound == UNKNOWN) {
    ida_star_solve(&GLOBAL_SOLVER.edge_ida, ldc, 0);
    lower_bound = GLOBAL_SOLVER.edge_ida.path_length - 1 + GLOBAL_SOLVER.edge_goal.num_sets - 1;
  }

  unsigned char goal_depth = GLOBAL_SOLVER.goal.num_sets - 1;
  if (lower_bound < goal_depth) {
    lower_bound = 0;
  } else {
    lower_bound -= goal_depth;
  }

  sequence first_steps = I;
  goal_depth = goalsphere_depth(&GLOBAL_SOLVER.goal, ldc, 0);
  if (goal_depth == UNKNOWN) {
    #ifdef _OPENMP
    ida_star_solve_parallel(&GLOBAL_SOLVER.ida, ldc, lower_bound);
    #else
    ida_star_solve(&GLOBAL_SOLVER.ida, ldc, lower_bound);
    #endif
    first_steps = ida_to_sequence(&GLOBAL_SOLVER.ida);
  }
  LocDirCube clone = *ldc;
  locdir_apply_sequence(&clone, first_steps);
  sequence final_steps = goalsphere_solve(&GLOBAL_SOLVER.goal, &clone, 0, &is_better);
  return concat(first_steps, final_steps);
}

void free_global_solver() {
  free_nibblebase(&GLOBAL_SOLVER.first);
  free_nibblebase(&GLOBAL_SOLVER.last);
  free_nibblebase(&GLOBAL_SOLVER.corners);
  free_goalsphere(&GLOBAL_SOLVER.goal);
  free_goalsphere(&GLOBAL_SOLVER.edge_goal);
}
