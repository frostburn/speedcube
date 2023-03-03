#include "assert.h"
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

typedef struct
{
  LocDirCube ldc;
  size_t hash;
} HashPair;

int cmp_hash_pair(const void *a, const void *b) {
  return cmp_size_t(&((HashPair*)a)->hash, &((HashPair*)b)->hash);
}

void test_hash_collisions() {
  printf("Checking for the quality of the hash...\n");
  size_t depth = 5;

  size_t size = 1;
  size_t accum = 1;
  for (size_t i = 0; i < depth; ++i) {
    accum *= NUM_STABLE_MOVES;
    size += accum;
  }
  HashPair *pairs = malloc(size * sizeof(HashPair));
  size_t num_pairs = 0;
  void accumulate(LocDirCube *ldc, size_t depth_) {
    pairs[num_pairs].ldc = *ldc;
    pairs[num_pairs].hash = locdir_centerless_hash(ldc);
    num_pairs++;
    if (depth_ <= 0) {
      return;
    }
    for (size_t i = 0; i < NUM_STABLE_MOVES; ++i) {
      LocDirCube child = *ldc;
      locdir_apply_stable(&child, STABLE_MOVES[i]);
      accumulate(&child, depth_ - 1);
    }
  }

  LocDirCube root;
  locdir_reset(&root);
  accumulate(&root, depth);

  qsort(pairs, num_pairs, sizeof(HashPair), cmp_hash_pair);

  size_t num_unique = 1;
  for (size_t i = 1; i < num_pairs; ++i) {
    if (pairs[i-1].hash == pairs[i].hash) {
      assert(locdir_equals(&pairs[i-1].ldc, &pairs[i].ldc));
    } else {
      assert(!locdir_equals(&pairs[i-1].ldc, &pairs[i].ldc));
      num_unique++;
    }
  }
  printf("No hash collisions found at depth %zu with %zu positions evaluated (%zu unique).\n", depth, num_pairs, num_unique);
  free(pairs);

  printf("Correlating with GoalSphere...\n");
  GoalSphere sphere = init_goalsphere(&root, depth, locdir_centerless_hash);
  size_t sphere_total = 0;
  for (size_t i = 0; i < sphere.num_sets; ++i) {
    sphere_total += sphere.set_sizes[i];
  }
  printf("%zu positions in the data structure.\n", sphere_total);

  assert(num_unique == sphere_total);
  free_goalsphere(&sphere);
}

void test_hashset() {
  // Fits five numbers in a set that takes the space of eight
  HashSet set = init_hashset(3, ~0ULL);
  hashset_add(&set, 2);
  hashset_add(&set, 3);
  hashset_add(&set, 5);
  hashset_add(&set, 7);
  hashset_add(&set, 11);
  assert(set.num_elements == 5);
  assert(set.magnitude == 3);
  assert(hashset_has(&set, 7));
  assert(!hashset_has(&set, 13));

  // Grows if needed
  hashset_add(&set, 13);
  hashset_add(&set, 17);
  hashset_add(&set, 19);
  hashset_add(&set, 23);
  assert(set.num_elements == 9);
  assert(set.magnitude == 4);
  assert(hashset_has(&set, 5));
  assert(hashset_has(&set, 13));
  assert(!hashset_has(&set, 29));

  free_hashset(&set);

  printf("All hash set tests pass!\n");
}

void test_locdir() {
  LocDirCube ldc;

  // U
  locdir_reset(&ldc);
  Cube cube = to_cube(&ldc);

  turn_U(&cube);

  locdir_U(&ldc);

  Cube clone = to_cube(&ldc);
  assert(equals(&cube, &clone));

  // D
  locdir_reset(&ldc);
  cube = to_cube(&ldc);

  turn_D(&cube);

  locdir_D(&ldc);

  clone = to_cube(&ldc);
  assert(equals(&cube, &clone));

  // R
  locdir_reset(&ldc);
  cube = to_cube(&ldc);

  turn_R(&cube);

  locdir_R(&ldc);

  clone = to_cube(&ldc);
  assert(equals(&cube, &clone));

  // L
  locdir_reset(&ldc);
  cube = to_cube(&ldc);

  turn_L(&cube);

  locdir_L(&ldc);

  clone = to_cube(&ldc);
  assert(equals(&cube, &clone));

  // y
  locdir_reset(&ldc);
  cube = to_cube(&ldc);

  rotate_y(&cube);

  locdir_y(&ldc);

  clone = to_cube(&ldc);
  assert(equals(&cube, &clone));

  // F
  locdir_reset(&ldc);
  cube = to_cube(&ldc);

  turn_F(&cube);

  locdir_F(&ldc);

  clone = to_cube(&ldc);
  assert(equals(&cube, &clone));

  // B
  locdir_reset(&ldc);
  cube = to_cube(&ldc);

  turn_B(&cube);

  locdir_B(&ldc);

  clone = to_cube(&ldc);
  assert(equals(&cube, &clone));

  // x
  locdir_reset(&ldc);
  cube = to_cube(&ldc);

  rotate_x(&cube);

  locdir_x(&ldc);

  clone = to_cube(&ldc);
  assert(equals(&cube, &clone));

  // z
  locdir_reset(&ldc);
  cube = to_cube(&ldc);

  rotate_z(&cube);

  locdir_z(&ldc);

  clone = to_cube(&ldc);
  assert(equals(&cube, &clone));

  for (size_t j = 0; j < 10; ++j) {
    locdir_reset(&ldc);

    for (size_t i = 0; i < 10; ++i) {
      int r = rand() % 3;
      if (r == 0) {
        locdir_x(&ldc);
      } else if (r == 1) {
        locdir_y(&ldc);
      } else {
        locdir_z(&ldc);
      }
    }

    locdir_realign(&ldc);
    for (size_t i = 0; i < 6; ++i) {
      assert(ldc.center_locs[i] == i);
    }
  }

  for (enum move move = I; move <= MAX_MOVE; ++move) {
    locdir_reset(&ldc);
    cube = to_cube(&ldc);
    locdir_apply(&ldc, move);
    apply(&cube, move);
    clone = to_cube(&ldc);
    assert(equals(&cube, &clone));
  }

  size_t indices[NUM_STABLE_MOVES];
  for (size_t i = 0; i < NUM_STABLE_MOVES; ++i) {
    locdir_reset(&ldc);
    // reset_oll(&cube);
    // apply(&cube, STABLE_MOVES[i]);
    // compensate(&cube, STABLE_MOVES[i]);
    locdir_apply_stable(&ldc, STABLE_MOVES[i]);
    indices[i] = locdir_oll_index(&ldc);
    // printf("%zu\n", indices[i]);
    // render(&cube);
    // printf("\n");
  }

  qsort(indices, NUM_STABLE_MOVES, sizeof(size_t), cmp_size_t);

  size_t num_unique = 1;
  for (size_t i = 1; i < NUM_STABLE_MOVES; ++i) {
    if (indices[i - 1] != indices[i]) {
      num_unique++;
    }
  }

  #if SCISSORS_ENABLED
  assert(num_unique == 34);
  #else
  assert(num_unique == 22);
  #endif

  printf("All locdir tests pass!\n");
}

int main() {
  srand(time(NULL));

  Cube cube = test_cube();
  assert(cube.a == 12009600080063147ULL);
  assert(cube.b == 15440913008127414ULL);
  assert(cube.c == 16852179250805726ULL);

  printf("Test cube:\n");
  render(&cube);

  cube = test_cube();
  rotate_x(&cube);
  assert(cube.a == 13147226992454378ULL);
  assert(cube.b == 7735506144291903ULL);
  assert(cube.c == 16848776528379325ULL);

  cube = test_cube();
  apply(&cube, U);
  assert(cube.a == 12009599812412589ULL);
  assert(cube.b == 15445448493591990ULL);
  assert(cube.c == 16852179117635549ULL);

  cube = test_cube();
  apply(&cube, D);
  assert(cube.a == 12009574360492907ULL);
  assert(cube.b == 17763081565990326ULL);
  assert(cube.c == 13368943610690270ULL);

  cube = test_cube();
  apply(&cube, R);
  assert(cube.a == 13151159303449259ULL);
  assert(cube.b == 5167086173384118ULL);
  assert(cube.c == 16852180289928158ULL);

  cube = test_cube();
  apply(&cube, L);
  assert(cube.a == 12013517090207918ULL);
  assert(cube.b == 15445890338352632ULL);
  assert(cube.c == 14635495089765243ULL);

  cube = test_cube();
  apply(&cube, F);
  assert(cube.a == 11904046963796879ULL);
  assert(cube.b == 15234204841013682ULL);
  assert(cube.c == 16878567529889274ULL);

  cube = test_cube();
  apply(&cube, B);
  assert(cube.a == 7505999707155114ULL);
  assert(cube.b == 1930191636753855ULL);
  assert(cube.c == 14600337293802463ULL);

  cube = test_cube();
  apply(&cube, U_prime);
  assert(cube.a == 12009599678195373ULL);
  assert(cube.b == 15415143204351414ULL);
  assert(cube.c == 16852178714459615ULL);

  cube = test_cube();
  apply(&cube, U2);
  assert(cube.a == 12009600080325290ULL);
  assert(cube.b == 15425863442722230ULL);
  assert(cube.c == 16852178983157723ULL);

  cube = test_cube();
  apply(&cube, u);
  assert(cube.a == 12009601955714197ULL);
  assert(cube.b == 15445448493591990ULL);
  assert(cube.c == 16852175894312957ULL);

  cube = test_cube();
  apply(&cube, d);
  assert(cube.a == 12009576501697371ULL);
  assert(cube.b == 17763081565990326ULL);
  assert(cube.c == 13368944684415734ULL);

  cube = test_cube();
  apply(&cube, r);
  assert(cube.a == 13143340583922347ULL);
  assert(cube.b == 5167086173384118ULL);
  assert(cube.c == 16853140215119838ULL);

  cube = test_cube();
  apply(&cube, l);
  assert(cube.a == 12005683069918382ULL);
  assert(cube.b == 15445890338352632ULL);
  assert(cube.c == 14566225857208187ULL);

  cube = test_cube();
  apply(&cube, f);
  assert(cube.a == 12748471893928733ULL);
  assert(cube.b == 15516229573013936ULL);
  assert(cube.c == 17724092001009018ULL);

  cube = test_cube();
  apply(&cube, b);
  assert(cube.a == 8350424607402666ULL);
  assert(cube.b == 2212216335723839ULL);
  assert(cube.c == 15725137722047439ULL);

  cube = test_cube();
  apply(&cube, M);
  assert(cube.a == 12001766059773611ULL);
  assert(cube.b == 15440913008127414ULL);
  assert(cube.c == 16782910018248670ULL);

  cube = test_cube();
  apply(&cube, E);
  assert(cube.a == 12009602221267611ULL);
  assert(cube.b == 15440913008127414ULL);
  assert(cube.c == 16852180324531190ULL);

  cube = test_cube();
  apply(&cube, S);
  assert(cube.a == 12854025010195001ULL);
  assert(cube.b == 15722937740127668ULL);
  assert(cube.c == 17697703721925470ULL);

  cube = test_cube();
  apply(&cube, M_prime);
  assert(cube.a == 12001781360536235ULL);
  assert(cube.b == 15440913008127414ULL);
  assert(cube.c == 16853139175997406ULL);

  for (int i = 0; i < 100; ++i) {
    Cube reference = test_cube();

    sequence seq = 0;
    int length = 1 + (rand() % SEQUENCE_MAX_LENGTH);
    for (int j = 0; j < length; ++j) {
      enum move move = rand() % NUM_MOVES;
      apply(&reference, move);
      seq = NUM_MOVES * seq + move;
    }

    cube = test_cube();
    apply_sequence(&cube, seq);
    assert(equals(&cube, &reference));
    // render(&cube);
    apply_sequence(&cube, invert(seq));
    assert(cube.a == 12009600080063147ULL);
    assert(cube.b == 15440913008127414ULL);
    assert(cube.c == 16852179250805726ULL);
  }

  sequence seq = from_moves((enum move[]){U, F2, U, M_prime, U2, M, U, F2, I});

  cube = test_cube();
  apply_sequence(&cube, seq);
  assert(cube.a == 12009599812411567ULL);
  assert(cube.b == 15445448493591990ULL);
  assert(cube.c == 16852179117111263ULL);

  sequence two = from_moves((enum move[]){b2, b2, I});
  sequence simple = from_moves((enum move[]){F, R, U_prime, I});
  sequence simpler = from_moves((enum move[]){U_prime, R, F, I});
  sequence complex = from_moves((enum move[]){S_prime, d2, l, I});
  sequence four = from_moves((enum move[]){U, U, U, U, I});

  assert(is_better(two, simple));
  assert(is_better(two, complex));
  assert(is_better(two, four));

  assert(is_better(simple, complex));
  assert(!is_better(complex, simple));

  assert(is_better(complex, four));

  assert(is_better(simpler, simple));
  assert(!is_better(simple, simpler));

  reset_oll(&cube);
  assert(cube.a == 35115784733176ULL);
  assert(cube.b == 17979214269771776ULL);
  assert(cube.c == 18014397435740160ULL);

  reset_cross(&cube);
  assert(cube.a == 37748880ULL);
  assert(cube.b == 6544293246345216ULL);
  assert(cube.c == 6544312535875584ULL);

  reset_f2l(&cube);
  assert(cube.a == 132121080ULL);
  assert(cube.b == 17979214269771776ULL);
  assert(cube.c == 17979281783128064ULL);
  assert(has_cross(&cube));

  reset_f2l_corners(&cube);
  assert(cube.a == 88080720ULL);
  assert(cube.b == 11997870970544128ULL);
  assert(cube.c == 11997915979448320ULL);

  reset_f2l_edges_a(&cube);
  assert(cube.a == 37748888ULL);
  assert(cube.b == 6544293246345216ULL);
  assert(cube.c == 6544316830842880ULL);

  reset_f2l_edges_b(&cube);
  assert(cube.a == 48234672ULL);
  assert(cube.b == 1970324885237760ULL);
  assert(cube.c == 1970328058200064ULL);

  // printf("assert(cube.a == %lluULL);\nassert(cube.b == %lluULL);\nassert(cube.c == %lluULL);\n", cube.a, cube.b, cube.c);
  // render(&cube);

  printf("All cube tests pass!\n");

  test_locdir();

  test_hashset();

  // test_hash_collisions();

  return EXIT_SUCCESS;
}
