#include <mpfr.h>
#include <limits>
#include <iostream>

int main() {
    mpfr_t one;
    mpfr_init2(one, 24);
    mpfr_set_str(one, "1", 2, GMP_RNDN);

    std::cout << mpfr_get_exp(one) << std::endl;
    mpfr_out_str(stdout, 2, 0, one, GMP_RNDN);
    std::cout << std::endl;
    mpfr_out_str(stdout, 10, 0, one, GMP_RNDN);
    std::cout << std::endl << std::endl;

    mpfr_t next;
    mpfr_init2(next, 24);
    mpfr_set_str(next, "1", 2, GMP_RNDN);
    mpfr_nextabove(next);

    std::cout << mpfr_get_exp(next) << std::endl;
    mpfr_out_str(stdout, 2, 0, next, GMP_RNDN);
    std::cout << std::endl;
    mpfr_out_str(stdout, 10, 0, next, GMP_RNDN);
    std::cout << std::endl << std::endl;

    mpfr_t epsilon;
    mpfr_init2(epsilon, 24);
    mpfr_sub(epsilon, next, one, GMP_RNDN);

    std::cout << mpfr_get_exp(epsilon) << std::endl;
    mpfr_out_str(stdout, 2, 0, epsilon, GMP_RNDN);
    std::cout << std::endl;
    mpfr_out_str(stdout, 10, 0, epsilon, GMP_RNDN);
    std::cout << std::endl << std::endl;

    mpfr_t minimum;
    mpfr_init2(minimum, 24);
    mpfr_set_d(minimum, std::numeric_limits<float>::min(), GMP_RNDN);

    std::cout << mpfr_get_exp(minimum) << std::endl;
    mpfr_out_str(stdout, 2, 0, minimum, GMP_RNDN);
    std::cout << std::endl;
    mpfr_out_str(stdout, 10, 0, minimum, GMP_RNDN);
    std::cout << std::endl << std::endl;

    mpfr_t maximum;
    mpfr_init2(maximum, 24);
    mpfr_set_d(maximum, std::numeric_limits<float>::max(), GMP_RNDN);

    std::cout << mpfr_get_exp(maximum) << std::endl;
    mpfr_out_str(stdout, 2, 0, maximum, GMP_RNDN);
    std::cout << std::endl;
    mpfr_out_str(stdout, 10, 0, maximum, GMP_RNDN);
    std::cout << std::endl << std::endl;
}