#ifndef _NTR_GRAPH_H_
#define _NTR_GRAPH_H_ 1

#include <raylib.h>
#include <complex.h>
#define comp double complex

#define W 1500
#define H 1000
#define SPACE 250
#define TICK_LEN 10
#define MARK_SZ 1

/*#define COL1 GREEN
#define COL2 RED
#define COL3 BLUE
#define COL4 YELLOW*/
#define COL1 RED
#define COL2 RED
#define COL3 RED
#define COL4 RED
#define COLNUM RED
//#define COL(N, M) (Color) { .r=255*(M-N)/(M), .b=255*(M)/(M-N), .a=255 }
#define COL(N, M) (Color) { .r=255, .b=0, .a=255 }

void drawAxes(void);
/** Compex to screen coordinates */
Vector2 coord(double complex c);

#endif // !_NTR_GRAPH_H_

