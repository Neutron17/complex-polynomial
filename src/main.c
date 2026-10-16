#include <assert.h>
#include <complex.h>
#include <stdbool.h>
#include <raylib.h>
#include <math.h>
#include "graphics.h"
#include "solver.h"

#define SCROLL_SPEED 10
#define ITER_COUNT 50
#define comp double complex

int main(int argc, char *argv[]) {
	InitWindow(W, H, "Andai");

	const comp a_n[] = { 1.0, -1.0, I };

	while(!WindowShouldClose()) {
		if(IsKeyPressed(KEY_Q))
			break;

		BeginDrawing();
		ClearBackground(BLACK);
		
		drawAxes();
		numerical(10, a_n, sizeof(a_n)/sizeof(*a_n), ITER_COUNT);

		EndDrawing();
	}

	CloseWindow();


	return 0;
}

