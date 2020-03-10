#include <RInside_C.h>

int main() {
    setupRinC();
	evalQuietlyInR("y <- 3");
	evalQuietlyInR("z <- 2.5");
	evalQuietlyInR("print(y*z)");
	evalQuietlyInR("y <- rnorm(10)");
	evalQuietlyInR("print(y)");
	SEXP vec = evalInR("y");
	Rf_PrintValue(vec);
	printf("%f\n", REAL(vec)[4]);
    teardownRinC();
}
