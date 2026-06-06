/*
 * Copyright (c) Ian Pike
 * Copyright (c) CCMath contributors
 *
 * CCMath is provided under the Apache-2.0 License WITH LLVM-exception.
 * See LICENSE for more information.
 *
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 */

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace
{
	std::filesystem::path repo_root()
	{
		auto current = std::filesystem::path(__FILE__).parent_path();
		while (!current.empty())
		{
			if (std::filesystem::exists(current / "include" / "ccmath")) { return current; }
			current = current.parent_path();
		}
		return std::filesystem::path(__FILE__).parent_path();
	}

	bool is_library_source_file(const std::filesystem::path & path)
	{
		const std::string ext = path.extension().string();
		return ext == ".hpp" || ext == ".h" || ext == ".ipp" || ext == ".inl";
	}
} // namespace

// ccmath requires that library implementation code under include/ccmath must not include <cmath>.
TEST(CcmathImplementationPolicyTests, LibraryHeadersDoNotIncludeCmath)
{
	const std::filesystem::path include_root = repo_root() / "include" / "ccmath";

	std::vector<std::string> violations;

	for (const auto & entry : std::filesystem::recursive_directory_iterator(include_root))
	{
		if (!entry.is_regular_file()) { continue; }
		if (!is_library_source_file(entry.path())) { continue; }

		std::ifstream input(entry.path());
		ASSERT_TRUE(input.is_open()) << "failed to open " << entry.path().string();

		std::string line;
		int line_number = 0;
		while (std::getline(input, line))
		{
			++line_number;
			if (line.find("#include <cmath>") != std::string::npos || line.find("#include<cmath>") != std::string::npos)
			{
				violations.push_back(entry.path().string() + ":" + std::to_string(line_number));
			}
		}
	}

	EXPECT_TRUE(violations.empty());
	if (!violations.empty())
	{
		for (const std::string & violation : violations)
		{
			ADD_FAILURE() << "forbidden <cmath> include in library header: " << violation;
		}
	}
}
