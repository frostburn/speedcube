# speedcube
Bitboard and location-direction a.k.a. index-orientation implementations of the 3x3x3 twisty puzzle in C.

## Results
OLL solutions with the least amount of face turns or slice moves: [OLL](https://frostburn.github.io/oll.html)

PLL solutions with the least amount of face turns or slice moves: [PLL](https://frostburn.github.io/pll.html)

## Solvers
 * `Nibblebase`: Tablebase of complete solutions to small subproblems where move count < 16 (a nibble).
 * `GoalSphere`: Lookup table of solutions up to a specified depth for problems too large to enumerate exhaustively.
 * `IDAstar`: [IDA*](https://en.wikipedia.org/wiki/Iterative_deepening_A*) implementation using tablebases as lower bounds to constrain the search in large problems.

Tablebases and lookup tables can be used to extract solutions that use the least amount of awkward moves. IDA* produces solutions that are as short as possible, but not necessarily easy to execute.

## Testing
```bash
gcc test.c -Ofast -o test.out && ./test.out
```

## Prerequisites
Compile the various tables used by the solvers.
```bash
gcc tabulate.c -Ofast -o tabulate.out && ./tabulate.out
```

## HTML generation
OLL
```bash
gcc oll_html.c -Ofast -o oll.out && ./oll.out > oll.html
```
PLL
```bash
gcc pll_html.c -Ofast -lm -o pll.out && ./pll.out > pll.html
```

## Inspired by
Benjamin Botto's [Medium article](https://medium.com/@benjamin.botto/implementing-an-optimal-rubiks-cube-solver-using-korf-s-algorithm-bf750b332cf9) and [bot](https://github.com/benbotto/rubiks-cube-cracker).
