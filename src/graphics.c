#include "graphics.h"
#include "complex.h"

Vector2 coord(double complex z) {
	return (Vector2) { 
		.x = (W/2.0 + SPACE * creal(z)),
		.y = (H/2.0 - SPACE * cimag(z))
	};
}

void drawAxes(void) {
	DrawLine(0, H/2, W, H/2, WHITE);
	DrawLine(W/2, 0, W/2, H, WHITE);
	for(int x = -W/10; x < W/10; x++) {
		DrawLine(W/2 - x*SPACE, H/2 - TICK_LEN/2, 
				W/2 - x*SPACE, H/2 + TICK_LEN/2, WHITE);
	}
	for(int y = -H/10; y < W/10; y++) {
		DrawLine(W/2 - TICK_LEN/2, H/2 - y*SPACE, 
				W/2 + TICK_LEN/2, H/2 - y*SPACE, WHITE);
	}
}

