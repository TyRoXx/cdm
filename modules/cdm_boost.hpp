#ifndef CDM_BOOST_HPP
#define CDM_BOOST_HPP

#include <silicium/file_operations.hpp>
#include <silicium/run_process.hpp>
#include <boost/lexical_cast.hpp>

//Boost is so large that the output of cp exceeds the 4 MB log limit of travis
//which aborts the build job then.
//On Windows, the console is so slow (especially in Virtualbox) that we want
//to avoid the megabytes of output from the Boost build process.
//TODO: Buffer the output in memory and save it somewhere if something fails.
#if defined(CDM_TESTS_RUNNING_ON_TRAVIS_CI) || defined(_WIN32)
#	define CDM_AVOID_CONSOLE_OUTPUT 1
#else
#	define CDM_AVOID_CONSOLE_OUTPUT 0
#endif

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
		boost::ignore_unused_variable_warning(cmake_exe);

		Si::absolute_path const module_in_cache = install_root / Si::relative_path("boost");
		if (!Si::file_exists(module_in_cache, Si::throw_))
		{
			Si::absolute_path const copy_of_boost = temporary / Si::relative_path("src");
			Si::copy_recursively(
				source,
				copy_of_boost,
#if CDM_AVOID_CONSOLE_OUTPUT
				nullptr
#else
				&output
#endif
				, Si::throw_);

			Si::create_directories(module_in_cache, Si::throw_);

			{
				std::vector<Si::os_string> arguments;
#ifdef _MSC_VER
				Si::absolute_path const exe = copy_of_boost / "bootstrap.bat";
#else
				Si::absolute_path const exe = *Si::absolute_path::create("/usr/bin/env");
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("bash"));
				arguments.push_back(Si::to_os_string(copy_of_boost / "bootstrap.sh"));
#endif
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("--prefix=") + Si::to_os_string(module_in_cache));
				int const rc = Si::run_process(exe, arguments, copy_of_boost, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("bootstrap failed");
				}
			}

			{
#if CDM_AVOID_CONSOLE_OUTPUT
				auto null_output = Si::Sink<char, Si::success>::erase(Si::null_sink<char, Si::success>());
#endif
				std::vector<Si::os_string> arguments;
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("install"));
#ifdef _MSC_VER
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("toolset=msvc-12.0"));
#endif
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-j ") + boost::lexical_cast<Si::os_string>(make_parallelism));
				int const rc = Si::run_process(copy_of_boost / "b2"
#ifdef _WIN32
					".exe"
#endif
					, arguments, copy_of_boost,
#if CDM_AVOID_CONSOLE_OUTPUT
					null_output
#else
					output
#endif
					).get();
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
