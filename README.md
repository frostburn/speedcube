# speedcube
Bitboard and location-direction a.k.a. index-orientation implementations of the 3x3x3 twisty puzzle in C.

## Results
Solutions with the least amount of face turns or slice moves. See [move notation](https://jperm.net/3x3/moves).

Solutions for the [first F2L pair](https://frostburn.github.io/first-f2l-pair.html).

Solutions for the [last F2L pair](https://frostburn.github.io/last-f2l-pair.html).

Orientation of the last layer: [OLL](https://frostburn.github.io/oll.html)

Permutation of the last layer: [PLL](https://frostburn.github.io/pll.html)

### With scissor moves
There is nothing mechanically preventing you from turning the top and bottom layers at the same time and such parallel execution can be counted as a single action resulting in the scissor move `[UD]`. With enough dexterity you can even turn the top layer 180° while only turning the bottom layer 90°, notated `[U2D]`. In rare cases you might even see the slice moves spelled in stable form like `E` replaced with `[D'U]`.

Assuming that all 90° and 180° turns take the same amount time these new parallelized moves result in faster algorithms.

Solutions for the [first F2L pair](https://frostburn.github.io/first-f2l-pair-scissors.html).

Solutions for the [last F2L pair](https://frostburn.github.io/last-f2l-pair-scissors.html).

Orientation of the last layer: [OLL](https://frostburn.github.io/oll-scissors.html)

Permutation of the last layer: [PLL](https://frostburn.github.io/pll-scissors.html)

## Solvers
 * `Nibblebase`: Tablebase of complete solutions to small subproblems where move count < 16 (a nibble).
 * `GoalSphere`: Lookup table of solutions up to a specified depth for problems too large to enumerate exhaustively.
 * `IDAstar`: [IDA*](https://en.wikipedia.org/wiki/Iterative_deepening_A*) implementation using tablebases as lower bounds to constrain the search in large problems.

Tablebases and lookup tables can be used to extract solutions that use the least amount of awkward moves. IDA* produces solutions that are as short as possible, but not necessarily easy to execute. There is an exhaustive search mode for A* (`global_solve_all`) which can be filtered down for the best solution to even the hardest scrambles.

## Testing
```bash
gcc test.c -lm -Ofast -o test.out && ./test.out
```

## Prerequisites
Compile the various tables used by the solvers.
```bash
gcc tabulate.c -lm -Ofast -o tabulate.out && ./tabulate.out
```

## HTML generation
First F2L pair
```bash
gcc first_f2l_pair_html.c -lm -Ofast -o first_f2l_pair.out && ./first_f2l_pair.out > first-f2l-pair.html
```
Last F2L pair
```bash
gcc last_f2l_pair_html.c -lm -Ofast -o last_f2l_pair.out && ./last_f2l_pair.out > last-f2l-pair.html
```
OLL
```bash
gcc oll_html.c -lm -Ofast -o oll.out && ./oll.out > oll.html
```
PLL
```bash
gcc pll_html.c -lm -Ofast -o pll.out && ./pll.out > pll.html
```

## Code quality issues
This is a hobbyist's throw-away project which is evident from the lack of a build system. To turn off scissor moves you need to edit `moves.c`.

## Inspired by
Benjamin Botto's [Medium article](https://medium.com/@benjamin.botto/implementing-an-optimal-rubiks-cube-solver-using-korf-s-algorithm-bf750b332cf9) and [bot](https://github.com/benbotto/rubiks-cube-cracker).
