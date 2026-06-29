#include "mpfr_fma_common.hpp"

int main(int argc, char ** argv)
{
	return ccm::test::oracle::fma_oracle::run_campaign<float>(argc, argv, "ccm::fmaf", 192);
}
