#pragma once

#include "mpfr_oracle_harness.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace ccm::test::oracle
{
	template <typename T>
	inline bool has_ternary_hard_failure(const std::vector<ternary_failure_record<T>>& events)
	{ return !events.empty(); }

	template <typename T>
	inline void write_ternary_failure_json(const std::string& output_path, const std::vector<ternary_failure_record<T>>& events)
	{
		std::ofstream out(output_path, std::ios::trunc);
		out << "[\n";
		for (std::size_t i = 0; i < events.size(); ++i)
		{
			const auto& event = events[i];
			out << "  {\n";
			out << "    \"function_name\": \"" << json_escape(event.function_name) << "\",\n";
			out << "    \"input_type\": \"" << json_escape(event.input_type) << "\",\n";
			out << "    \"path\": \"" << json_escape(event.path) << "\",\n";
			out << "    \"provenance\": \"" << json_escape(event.provenance) << "\",\n";
			out << "    \"x_bits\": \"" << event.x_bits << "\",\n";
			out << "    \"y_bits\": \"" << event.y_bits << "\",\n";
			out << "    \"z_bits\": \"" << event.z_bits << "\",\n";
			out << "    \"actual_bits\": \"" << event.actual_bits << "\",\n";
			out << "    \"expected_bits\": \"" << event.expected_bits << "\",\n";
			out << "    \"ulp_distance\": " << event.ulp_distance << ",\n";
			out << "    \"rounding_mode\": \"" << json_escape(event.rounding_mode) << "\",\n";
			out << "    \"oracle_precision\": " << event.oracle_precision << ",\n";
			out << "    \"compiler\": \"" << json_escape(event.compiler) << "\",\n";
			out << "    \"platform\": \"" << json_escape(event.platform) << "\",\n";
			out << "    \"fma_enabled\": \"" << json_escape(event.fma_enabled) << "\",\n";
			out << "    \"builtin_available\": \"" << json_escape(event.builtin_available) << "\",\n";
			out << "    \"configuration_name\": \"" << json_escape(event.configuration_name) << "\",\n";
			out << "    \"optimization_mode\": \"" << json_escape(event.optimization_mode) << "\",\n";
			out << "    \"seed\": " << event.seed << ",\n";
			out << "    \"timestamp\": \"" << json_escape(event.timestamp) << "\",\n";
			out << "    \"event_kind\": \"" << json_escape(event.event_kind) << "\",\n";
			out << "    \"notes\": \"" << json_escape(event.notes) << "\"\n";
			out << "  }" << (i + 1 == events.size() ? "\n" : ",\n");
		}
		out << "]\n";
	}

	template <typename T>
	inline void print_ternary_hard_failures(const std::vector<ternary_failure_record<T>>& events)
	{
		for (const auto& event : events)
		{
			std::cout << "HARD FAILURE " << event.event_kind << " fn=" << event.function_name << " path=" << event.path << " rounding=" << event.rounding_mode
					  << " x=" << event.x_bits << " y=" << event.y_bits << " z=" << event.z_bits << " actual=" << event.actual_bits
					  << " expected=" << event.expected_bits << " ulp=" << event.ulp_distance << " provenance=\"" << event.provenance << "\" notes=\""
					  << event.notes << "\"\n";
		}
	}
} // namespace ccm::test::oracle
