#include "oracle/runners/mpfr_pow_runner.hpp"

int main(int argc, char ** argv)
{
	return ccm::test::oracle::pow_mpfr::run_campaign<float>(argc, argv);
}
