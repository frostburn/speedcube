#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "stdbool.h"

#include "cube.c"
#include "moves.c"
#include "sequence.c"

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

  // printf("assert(cube.a == %lluULL);\nassert(cube.b == %lluULL);\nassert(cube.c == %lluULL);\n", cube.a, cube.b, cube.c);
  // render(&cube);

  for (int i = 0; i < 100; ++i) {
    Cube reference = test_cube();

    sequence seq = 0;
    int length = 1 + (rand() % SEQUENCE_MAX_LENGTH);
    for (int j = 0; j < length; ++j) {
      enum move m = rand() % NUM_MOVES;
      apply(&reference, m);
      seq = NUM_MOVES * seq + m;
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

  printf("All tests pass!\n");

  reset_oll(&cube);
  assert(cube.a == 35115784733176ULL);
  assert(cube.b == 17979214269771776ULL);
  assert(cube.c == 18014397435740160ULL);

  reset_cross(&cube);
  assert(cube.a == 37748880ULL);
  assert(cube.b == 6544293246345216ULL);
  assert(cube.c == 6544312535875584ULL);

  return EXIT_SUCCESS;
}
