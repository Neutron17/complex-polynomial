#include <assert.h>
#include <complex.h>
#include <stdio.h>
#include <stdbool.h>
#include <raylib.h>
#include <math.h>
#include <string.h>

#define W 1500
#define H 1000
#define SPACE 250
#define TICK_LEN 10
#define MARK_SZ 1
#define SCROLL_SPEED 10
#define ITER_COUNT 8
#define comp double complex
/*#define COL1 GREEN
#define COL2 RED
#define COL3 BLUE
#define COL4 YELLOW*/
#define COL1 RED
#define COL2 RED
#define COL3 RED
#define COL4 RED
#define COLNUM RED
#define COL(N, M) (Color) { .r=255*(M-N)/(M), .b=255*(M)/(M-N), .a=255 }
#define CPRINT(Z) printf("%lf+%lfi\n", creal(Z), cimag(Z))
#define EPS 1e-14

void drawAxes(void);
/** Compex to screen coordinates */
Vector2 coord(comp);

void lin(comp a, comp b);
void quad(comp a, comp b, comp c);
void cubic(comp a, comp b, comp c, comp d);
void quartic(comp a, comp b, comp c, comp d, comp e);

/** Evaluates a_n z^n + ... + a_{1} z + a_0 at z
 * using Horner's rule */
comp horner_method(int n, comp *a, comp z);
/** Horner's method for the polynomial's derivative */
comp horner_method_deriv(int n, comp *a, comp z);
void numerical(int n, const comp *const a_n, int a_sz);

#define DrawCircleV(P,SZ,C) (DrawPixelV(P,C))

int main(int argc, char *argv[]) {
	InitWindow(W, H, "Andai");
	//SetTargetFPS(30);

	const comp a_n[3] = { 1,2,I };
	/*comp b_n[2] = { 2, I };
	comp c_n[2] = { 1, I };
	comp d_n[2] = { 4, I };*/

	while(!WindowShouldClose()) {
		if(IsKeyPressed(KEY_Q))
			break;
		//((int)GetMouseWheelMove())*SCROLL_SPEED;

		BeginDrawing();
		ClearBackground(BLACK);
		

		drawAxes();
		for(int a = 0; a < sizeof(a_n)/sizeof(*a_n); a++) {
			for(int b = 0; b < sizeof(a_n)/sizeof(*a_n); b++) {
				lin(a_n[a],a_n[b]);
				//lin(b_n[a],b_n[b]);
				for(int c = 0; c < sizeof(a_n)/sizeof(*a_n); c++) {
					quad(a_n[a],a_n[b],a_n[c]);
					/*quad(b_n[a],b_n[b],b_n[c]);
					quad(c_n[a],c_n[b],c_n[c]);
					quad(d_n[a],d_n[b],d_n[c]);*/
					for(int d = 0; d < sizeof(a_n)/sizeof(*a_n); d++) {
						cubic(a_n[a], a_n[b], a_n[c], a_n[d]);
						/*cubic(b_n[a], b_n[b], b_n[c], b_n[d]);
						cubic(c_n[a], c_n[b], c_n[c], c_n[d]);
						cubic(d_n[a], d_n[b], d_n[c], d_n[d]);*/
						for(int e = 0; e < sizeof(a_n)/sizeof(*a_n); e++) {
							quartic(a_n[a], a_n[b], a_n[c], a_n[d], a_n[e]);
							/*quartic(b_n[a], b_n[b], b_n[c], b_n[d], b_n[e]);
							quartic(c_n[a], c_n[b], c_n[c], c_n[d], c_n[e]);
							quartic(d_n[a], d_n[b], d_n[c], d_n[d], d_n[e]);*/
						}
					}
				}
			}
		}
		for(int n = 5; n <= 10; n++) {
			numerical(n, a_n, sizeof(a_n)/sizeof(*a_n));
			/*numerical(n, b_n);
			numerical(n, c_n);
			numerical(n, d_n);*/
		}

		EndDrawing();
	}

	CloseWindow();


	return 0;
}

void numerical(int n, const comp *const a_n, int a_sz) {
	assert(a_n);
	assert(n>2);
	// polynomial coefficients
	comp b_n[n+1];
	// root guesses
	comp z_n[n];
	comp z_old[n];
	int max = pow(a_sz, n);
	//for(unsigned i = 0; i < (1u<<(n+1)); i++) {
		//for(int k = 0; k <= n; k++)
			//b_n[k] = a_n[(i&(1u<<k))!=0 ? 0 : 1];
	for(unsigned i = 0; i < max; i++) {
		unsigned x = i;
		for(int k = 0; k <= n; k++) {
			b_n[k] = a_n[x % a_sz];
			x /= a_sz;
		}

		// Cauchy bound: 1+max(b_j)
		double R = 1.0; 
		for(int j = 1; j <= n; j++)
			R = fmax(R, cabs(b_n[j] / b_n[0]));
		R++;

		// first guesses in a circle
		for(int k = 0; k<n; k++)
			z_n[k] = R*cexp(2*PI*I*k/n);

		for(int aberth = 0; aberth < ITER_COUNT; aberth++) {
			memcpy(z_old, z_n, n*sizeof(*z_n));
			bool converged = true;

			for(int k = 0; k < n; k++) {
				comp z = z_old[k];
				const comp p = horner_method(n, b_n, z);
				const comp p_prime = horner_method_deriv(n, b_n, z);

				comp S = 0.0;
				for(int j = 0; j < n; j++) {
					if(j == k)
						continue;
					S += 1.0/(z - z_old[j]);
				}
				const comp delta = 1.0/(p_prime/p - S);
				if(cabs(delta) >= EPS)
					converged = false;
				z_n[k] = z - delta;
			}
			if(converged)
				break;
		}
		for(int k = 0; k < n; k++) {
			Vector2 pos = coord(z_n[k]);
			DrawCircle(pos.x, pos.y, MARK_SZ, COL(n, 16));
		}
	}
}

comp horner_method_deriv(int n, comp *a, comp z) {
	comp partial = n*a[0];
	for(int i = 1; i < n; i++) {
		partial = (n-i)*a[i] + z*partial;
	}
	return partial;
}

/** Evaluates a_n z^n + ... + a_{1} z + a_0 at z */
comp horner_method(int n, comp *a, comp z) {
	comp partial = a[0];

	for(int i = 1; i < n+1; i++)
		partial = a[i] + z*partial;

	return partial;
}

void quartic(comp a, comp b, comp c, comp d, comp e) {
	const comp p = (8*a*c - 3*b*b)/(8*a*a);
	const comp q = (b*b*b - 4*a*b*c + 4*a*a*d)/(8*a*a*a);
	const comp d0 = c*c - 3*b*d + 12*a*e;
	const comp d1 = 2*c*c*c - 9*b*c*d + 27*b*b*e + 27*a*d*d - 72*a*c*e;
	const comp Q = cpow((d1+csqrt(d1*d1 - 4*d0*d0*d0))/2.0, 1.0/3.0);
	const comp S = csqrt(-(2.0/3.0)*p + 1.0/(3.0*a)*(Q + d0/Q))/2.0;

	Vector2 x0 = coord(-b/(4*a) - S + csqrt(-4*S*S - 2*p + q/S)/2.0);
	Vector2 x1 = coord(-b/(4*a) - S - csqrt(-4*S*S - 2*p + q/S)/2.0);
	Vector2 x2 = coord(-b/(4*a) - S + csqrt(-4*S*S - 2*p - q/S)/2.0);
	Vector2 x3 = coord(-b/(4*a) - S - csqrt(-4*S*S - 2*p - q/S)/2.0);

	DrawCircleV(x0, MARK_SZ, COL4);
	DrawCircleV(x1, MARK_SZ, COL4);
	DrawCircleV(x2, MARK_SZ, COL4);
	DrawCircleV(x3, MARK_SZ, COL4);
}

void cubic(comp a, comp b, comp c, comp d) {
	comp d0 = b*b - 3*a*c;
	comp d1 = 2*b*b*b - 9*a*b*c + 27*a*a*d;
	comp C = cpow((d1+csqrt(d1*d1 - 4*d0*d0*d0))/2.0, 1.0/3.0);

	comp xi1 = (-1.0 + I*sqrt(3.0))/2.0;
	comp xi2 = (-1.0 - I*sqrt(3.0))/2.0;

	
	if(cabs(C) > EPS) {
		comp x0 = -1.0/(3*a) * (b + C + d0/(C));
		comp x1 = -1.0/(3*a) * (b + xi1*C + d0/(xi1*C));
		comp x2 = -1.0/(3*a) * (b + xi2*C + d0/(xi2*C));
		/*DrawCircleV(coord(x0), MARK_SZ+2, GREEN);
		DrawCircleV(coord(x1), MARK_SZ+2, GREEN);
		DrawCircleV(coord(x2), MARK_SZ+2, GREEN);*/

		DrawCircleV(coord(x0), MARK_SZ, COL3);
		DrawCircleV(coord(x1), MARK_SZ, COL3);
		DrawCircleV(coord(x2), MARK_SZ, COL3);
	} else {
		printf("DEGENERATE CUBIC: abcd");
		CPRINT(a);
		CPRINT(b);
		CPRINT(c);
		CPRINT(d);
	}
}

void quad(comp a, comp b, comp c) {
	Vector2 x0 = coord((-b + csqrt(b*b - 4*a*c))/(2*a));
	Vector2 x1 = coord((-b - csqrt(b*b - 4*a*c))/(2*a));

	DrawCircle(x0.x, x0.y, MARK_SZ, COL2);
	DrawCircle(x1.x, x1.y, MARK_SZ, COL2);
}
// az + b = 0
void lin(comp a, comp b) {
	const Vector2 ans = coord(-b/a);
	DrawCircle(ans.x, ans.y, MARK_SZ, COL1);
}

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

