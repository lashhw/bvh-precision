#include <mpfr.h>
#include <limits>
#include <cassert>
#include <iostream>

void print_mpfr(const mpfr_t &num) {
    std::cout << mpfr_get_exp(num) << std::endl;
    mpfr_out_str(stdout, 2, 0, num, GMP_RNDN);
    std::cout << std::endl;
    mpfr_out_str(stdout, 10, 0, num, GMP_RNDN);
    std::cout << std::endl << std::endl;
}

int main() {
    mpfr_t one;
    mpfr_init2(one, 24);
    assert(mpfr_set_str(one, "1", 2, GMP_RNDN) == 0);
    print_mpfr(one);

    mpfr_t next;
    mpfr_init2(next, 24);
    assert(mpfr_set(next, one, GMP_RNDN) == 0);
    mpfr_nextabove(next);
    print_mpfr(next);

    mpfr_t epsilon;
    mpfr_init2(epsilon, 24);
    assert(mpfr_sub(epsilon, next, one, GMP_RNDN) == 0);
    print_mpfr(epsilon);

    mpfr_t minimum;
    mpfr_init2(minimum, 24);
    assert(mpfr_set_d(minimum, std::numeric_limits<float>::min(), GMP_RNDN) == 0);
    print_mpfr(minimum);

    mpfr_t maximum;
    mpfr_init2(maximum, 24);
    assert(mpfr_set_d(maximum, std::numeric_limits<float>::max(), GMP_RNDN) == 0);
    print_mpfr(maximum);
}