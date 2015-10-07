#include "testing.h"
#include <stdio.h>

static int _failure_count;

void real_print_test(const char* mensaje, bool ok,
		     const char* file, int line, const char* failed_expr) {
    if (ok) {
	printf("%s... OK\n", mensaje);
    } else {
	printf("%s: ERROR\n"  "%s:%d: %s\n",
	       mensaje, file, line, failed_expr);
    }
    fflush(stdout);
    _failure_count += !ok;
}

int failure_count() {
    return _failure_count;
}
