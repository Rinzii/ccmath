#include "oracle/runners/coremath_pow_runner.hpp"

int main(int argc, char ** argv)
{
	return ccm::test::oracle::pow_coremath::run_campaign<double>(argc, argv);
}
