#ifndef CDM_BOOST_HPP
#define CDM_BOOST_HPP

#include <silicium/file_operations.hpp>
#include <silicium/run_process.hpp>
#include <boost/lexical_cast.hpp>

namespace cdm
{
	struct boost_paths
	{
		Si::absolute_path root;
	};

	inline boost_paths install_boost(
		Si::absolute_path const &source,
		Si::absolute_path const &temporary,
		Si::absolute_path const &install_root,
		Si::absolute_path const &cmake_exe,
		unsigned make_parallelism,
		Si::Sink<char, Si::success>::interface &output)
	{
		boost::ignore_unused(cmake_exe);

		Si::absolute_path const module_in_cache = install_root / Si::relative_path("boost");
		if (!Si::file_exists(module_in_cache, Si::throw_))
		{
			Si::absolute_path const copy_of_boost = temporary / Si::relative_path("boost-copy");
			Si::copy_recursively(source, copy_of_boost, &output, Si::throw_);

			{
				std::vector<Si::os_string> arguments;
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("bash"));
				arguments.push_back(Si::to_os_string(copy_of_boost / "bootstrap.sh"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("--prefix=") + Si::to_os_string(module_in_cache));
				int const rc = Si::run_process(*Si::absolute_path::create("/usr/bin/env"), arguments, copy_of_boost, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("bootstrap failed");
				}
			}

			{
				std::vector<Si::os_string> arguments;
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-j ") + boost::lexical_cast<Si::os_string>(make_parallelism));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("install"));
				int const rc = Si::run_process(copy_of_boost / "b2", arguments, copy_of_boost, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("b2 failed");
				}
			}
		}
		boost_paths result;
		result.root = module_in_cache;
		return result;
	}
}

#endif
