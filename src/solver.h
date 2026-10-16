#include <complex.h>
#include <stdio.h>

#define comp double complex
#define CPRINT(Z) printf("%lf+%lfi\n", creal(Z), cimag(Z))

/** Evaluates a_n z^n + ... + a_{1} z + a_0 at z */
comp horner_method(int n, comp *a, comp z);
comp horner_method_deriv(int n, comp *a, comp z);

// az + b = 0
void lin(comp a, comp b);
void quad(comp a, comp b, comp c);
void cubic(comp a, comp b, comp c, comp d);
void quartic(comp a, comp b, comp c, comp d, comp e);

void numerical(int n, const comp *const a_n, int a_sz, int ITER_COUNT);

