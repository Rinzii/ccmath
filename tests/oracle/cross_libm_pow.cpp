#include "cross_libm_common.hpp"
#include "mpfr_pow_common.hpp"

#include "utils/worst_case_samples.hpp"

#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <vector>

namespace
{
	using ccm::test::oracle::cross_libm::backend_info;
	using ccm::test::oracle::cross_libm::disagreement_kind;

	struct case_record
	{
		double base;
		double exponent;
		std::map<std::string, double> results;
	};

	std::vector<case_record> build_corpus(const std::string & corpus)
	{
		std::vector<case_record> cases;
		for (const auto & hard : ccm::test::worst_case::kPowDoubleHard)
		{
			cases.push_back({ hard.base, hard.exponent, {} });
		}
		if (corpus == "extended")
		{
			for (double base : { 0.5, 1.0, 2.0, -2.0, 1e-300, 1e300 })
			{
				for (double exponent : { -3.0, -0.5, 0.0, 0.5, 2.0, 10.0 })
				{
					cases.push_back({ base, exponent, {} });
				}
			}
		}
		return cases;
	}

	double eval_backend(const backend_info & backend, double base, double exponent)
	{
		if (backend.id == "std_pow" || backend.id == "platform_libm") { return std::pow(base, exponent); }
		return std::pow(base, exponent);
	}
} // namespace

int main(int argc, char ** argv)
{
	const std::string corpus = ccm::test::oracle::option_value(argc, argv, "--corpus=").value_or("quick");
	const std::string format = ccm::test::oracle::option_value(argc, argv, "--format=").value_or("json");
	const std::string output = ccm::test::oracle::option_value(argc, argv, "--output=").value_or("cross-libm-pow.json");
	const bool fail_on_disagreement = ccm::test::oracle::has_flag(argc, argv, "--fail-on-disagreement");

	auto backends = ccm::test::oracle::cross_libm::detect_backends();
	std::vector<backend_info> active;
	for (const auto & backend : backends)
	{
		if (backend.available) { active.push_back(backend); }
	}

	auto cases = build_corpus(corpus);
	std::size_t disagreement_count = 0;

	std::ofstream out(output, std::ios::trunc);
	if (format == "csv")
	{
		out << "base,exponent,backend_a,backend_b,kind,result_a,result_b\n";
	}
	else
	{
		out << "[\n";
	}

	bool first_json = true;
	for (auto & test_case : cases)
	{
		for (const auto & backend : active)
		{
			test_case.results[backend.id] = eval_backend(backend, test_case.base, test_case.exponent);
		}

		const double mpfr_ref = ccm::test::oracle::mpfr_pow_reference(
			test_case.base, test_case.exponent, 256, ccm::test::oracle::current_mpfr_rounding_mode());

		for (std::size_t i = 0; i < active.size(); ++i)
		{
			for (std::size_t j = i + 1; j < active.size(); ++j)
			{
				const auto kind = ccm::test::oracle::cross_libm::classify(
					test_case.results[active[i].id], test_case.results[active[j].id]);
				if (kind == disagreement_kind::exact_match) { continue; }
				++disagreement_count;
				if (format == "csv")
				{
					out << test_case.base << ',' << test_case.exponent << ',' << active[i].id << ',' << active[j].id << ','
						<< ccm::test::oracle::cross_libm::disagreement_name(kind) << ',' << test_case.results[active[i].id] << ','
						<< test_case.results[active[j].id] << '\n';
				}
				else
				{
					if (!first_json) { out << ",\n"; }
					first_json = false;
					out << "  {\n";
					out << "    \"base_bits\": \"" << ccm::test::oracle::bits_hex(test_case.base) << "\",\n";
					out << "    \"exponent_bits\": \"" << ccm::test::oracle::bits_hex(test_case.exponent) << "\",\n";
					out << "    \"backend_a\": \"" << active[i].id << "\",\n";
					out << "    \"backend_b\": \"" << active[j].id << "\",\n";
					out << "    \"kind\": \"" << ccm::test::oracle::cross_libm::disagreement_name(kind) << "\",\n";
					out << "    \"result_a_bits\": \"" << ccm::test::oracle::bits_hex(test_case.results[active[i].id]) << "\",\n";
					out << "    \"result_b_bits\": \"" << ccm::test::oracle::bits_hex(test_case.results[active[j].id]) << "\",\n";
					out << "    \"mpfr_bits\": \"" << ccm::test::oracle::bits_hex(mpfr_ref) << "\"\n";
					out << "  }";
				}
			}
		}
	}

	if (format != "csv") { out << "\n]\n"; }

	std::cout << "cross-libm pow corpus=" << corpus << " active_backends=" << active.size() << " disagreements=" << disagreement_count
			  << " output=" << output << '\n';
	for (const auto & backend : backends)
	{
		std::cout << "  backend " << backend.id << " available=" << (backend.available ? "yes" : "no") << " label=" << backend.label << '\n';
	}

	if (fail_on_disagreement && disagreement_count > 0) { return 1; }
	return 0;
}
