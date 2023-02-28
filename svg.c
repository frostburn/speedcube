char *sticker_fill(Cube *cube, bitboard p) {
  bitboard red = cube->a & ~cube->b & ~cube->c;
  bitboard green = ~cube->a & cube->b & ~cube->c;
  bitboard orange = cube->a & cube->b & ~cube->c;
  bitboard blue = ~cube->a & ~cube->b & cube->c;
  bitboard yellow = cube->a & ~cube->b & cube->c;
  bitboard white = ~cube->a & cube->b & cube->c;
  bitboard purple = cube->a & cube->b & cube->c;
  if (p & red) {
    return "red";
  }
  if (p & green) {
    return "lime";
  }
  if (p & orange) {
    return "orange";
  }
  if (p & blue) {
    return "blue";
  }
  if (p & yellow) {
    return "yellow";
  }
  if (p & white) {
    return "white";
  }
  if (p & purple) {
    return "purple";
  }
  return "gray";
}

const double FR_HEIGHT = 50;
const double FL_HEIGHT = 45;

const double UFL_X = 5;
const double UFL_Y = 30;

const double UFR_X = 50;
const double UFR_Y = 45;

const double DFL_X = 7;
const double DFL_Y = UFL_Y + FL_HEIGHT;

const double DFR_X = 50;
const double DFR_Y = UFR_Y + FR_HEIGHT;

const double UBR_X = 100 - UFL_X;
const double UBR_Y = UFL_Y;

const double DBR_X = 100 - DFL_X;
const double DBR_Y = DFL_Y;

const double UBL_X = 50;
const double UBL_Y = 15;

double Fx(double i, double j) {
  double u = UFL_X + (UFR_X - UFL_X) * i / 3;
  double d = DFL_X + (DFR_X - DFL_X) * i / 3;
  return u + (d - u) * j / 3;
}

double Fy(double i, double j) {
  double l = UFL_Y + (DFL_Y - UFL_Y) * j / 3;
  double r = UFR_Y + (DFR_Y - UFR_Y) * j / 3;
  return l + (r - l) * i / 3;
}

double Rx(double i, double j) {
  double u = UFR_X + (UBR_X - UFR_X) * i / 3;
  double d = DFR_X + (DBR_X - DFR_X) * i / 3;
  return u + (d - u) * j / 3;
}

double Ry(double i, double j) {
  double l = UFR_Y + (DFR_Y - UFR_Y) * j / 3;
  double r = UBR_Y + (DBR_Y - UBR_Y) * j / 3;
  return l + (r - l) * i / 3;
}

double Ux(double i, double j) {
  double b = UBL_X + (UBR_X - UBL_X) * i / 3;
  double f = UFL_X + (UFR_X - UFL_X) * i / 3;
  return b + (f - b) * j / 3;
}

double Uy(double i, double j) {
  double l = UBL_Y + (UFL_Y - UBL_Y) * j / 3;
  double r = UBR_Y + (UFR_Y - UBR_Y) * j / 3;
  return l + (r - l) * i / 3;
}

void ufr_svg(LocDirCube *ldc) {
  Cube cube = to_cube(ldc);
  bitboard p;
  char *fill;
  printf("<svg viewBox=\"0 0 100 100\" width=\"100%%\" height=\"100%%\">\n");

  printf(" <g stroke=\"black\" stroke-width=\"1.1\" stroke-linejoin=\"bevel\">\n");
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      // F face
      p = 1ULL << (9 + i + 3*j);
      fill = sticker_fill(&cube, p);
      printf(
        "  <polygon points=\"%g,%g %g,%g %g,%g, %g,%g\" fill=\"%s\"/>\n",
        Fx(i+0, j+0), Fy(i+0, j+0),
        Fx(i+1, j+0), Fy(i+1, j+0),
        Fx(i+1, j+1), Fy(i+1, j+1),
        Fx(i+0, j+1), Fy(i+0, j+1),
        fill
      );

      // R face
      p = 1ULL << (18 + i + 3*j);
      fill = sticker_fill(&cube, p);

      printf(
        "  <polygon points=\"%g,%g %g,%g %g,%g, %g,%g\" fill=\"%s\"/>\n",
        Rx(i+0, j+0), Ry(i+0, j+0),
        Rx(i+1, j+0), Ry(i+1, j+0),
        Rx(i+1, j+1), Ry(i+1, j+1),
        Rx(i+0, j+1), Ry(i+0, j+1),
        fill
      );

      // U face
      p = 1ULL << (4*9 + i + 3*j);
      fill = sticker_fill(&cube, p);

      printf(
        "  <polygon points=\"%g,%g %g,%g %g,%g, %g,%g\" fill=\"%s\"/>\n",
        Ux(i+0, j+0), Uy(i+0, j+0),
        Ux(i+1, j+0), Uy(i+1, j+0),
        Ux(i+1, j+1), Uy(i+1, j+1),
        Ux(i+0, j+1), Uy(i+0, j+1),
        fill
      );
    }
  }
  printf(" </g>");
  printf("</svg>\n");
}
