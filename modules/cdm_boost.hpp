#ifndef CDM_BOOST_HPP
#define CDM_BOOST_HPP

#include <ventura/file_operations.hpp>
#include <ventura/run_process.hpp>
#include <silicium/sink/iterator_sink.hpp>
#include <boost/lexical_cast.hpp>

namespace cdm
{
	struct boost_paths
	{
		ventura::absolute_path root;
	};

	inline boost_paths install_boost(ventura::absolute_path const &source, ventura::absolute_path const &temporary,
	                                 ventura::absolute_path const &install_root, unsigned make_parallelism,
	                                 Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const module_in_cache = install_root / "boost";
		if (!ventura::file_exists(module_in_cache, Si::throw_))
		{
			ventura::absolute_path const copy_of_boost = temporary / "src";
			ventura::copy_recursively(source, copy_of_boost, &output, Si::throw_);

			ventura::create_directories(module_in_cache, Si::throw_);

			{
				std::vector<Si::os_string> arguments;
#ifdef _MSC_VER
				ventura::absolute_path const exe = copy_of_boost / "bootstrap.bat";
#else
				ventura::absolute_path const exe = *ventura::absolute_path::create("/usr/bin/env");
				arguments.emplace_back(SILICIUM_OS_STR("bash"));
				arguments.emplace_back(to_os_string(copy_of_boost / "bootstrap.sh"));
#endif
				int const rc = ventura::run_process(exe, arguments, copy_of_boost, output,
				                                    std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
				                                    ventura::environment_inheritance::inherit)
				                   .get();
				if (rc != 0)
				{
					throw std::runtime_error("bootstrap failed");
				}
			}

			{
				std::vector<Si::os_string> arguments;
				arguments.emplace_back(SILICIUM_OS_STR("install"));
				{
					Si::os_string const install_argument = SILICIUM_OS_STR("--prefix=") + to_os_string(module_in_cache);
					arguments.emplace_back(install_argument);
				}
				arguments.emplace_back(SILICIUM_OS_STR("link=static"));
#ifdef _MSC_VER
				arguments.emplace_back(
#if _MSC_VER == 1900
				    SILICIUM_OS_STR("toolset=msvc-14.0")
#elif _MSC_VER == 1800
				    SILICIUM_OS_STR("toolset=msvc-12.0")
#else
#error unsupported version of Visual Studio
#endif
				        );
#endif
#if CDM_TESTS_RUNNING_ON_TRAVIS_CI
				// GCC 4.6 crashes when compiling Boost.Log on travis probably due
				// to lack of RAM.
				// Thus we do not parallelize the build on travis so that the
				// compiler can use all of the memory available to the machine.
				boost::ignore_unused_variable_warning(make_parallelism);
#else
				arguments.emplace_back(SILICIUM_OS_STR("-j ") + boost::lexical_cast<Si::os_string>(make_parallelism));
#endif
				int const rc = ventura::run_process(copy_of_boost / "b2"
#ifdef _WIN32
				                                                    ".exe"
#endif
				                                    ,
				                                    arguments, copy_of_boost, output,
				                                    std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
				                                    ventura::environment_inheritance::inherit)
				                   .get();
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

	template <class StringSink>
	void generate_cmake_definitions_for_using_boost(StringSink &&definitions, ventura::absolute_path const &boost_root)
	{
		Si::append(definitions, "-DBOOST_ROOT=" + ventura::to_utf8_string(boost_root));
		Si::append(definitions, "-DBoost_ADDITIONAL_VERSIONS=1.59");
		Si::append(definitions, "-DBoost_NO_SYSTEM_PATHS=ON");
	}
}

#endif
