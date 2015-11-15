#ifndef CDM_CPPNETLIB_HPP
#define CDM_CPPNETLIB_HPP

#include "cdm_boost.hpp"
#include <cdm/cmake_generator.hpp>
#include <ventura/file_operations.hpp>
#include <ventura/run_process.hpp>
#include <boost/lexical_cast.hpp>

namespace cdm
{
	struct cppnetlib_paths
	{
		ventura::absolute_path cmake_prefix_path;
	};

	inline cppnetlib_paths install_cppnetlib(ventura::absolute_path const &cppnetlib_source,
	                                         ventura::absolute_path const &boost_source,
	                                         ventura::absolute_path const &temporary,
	                                         ventura::absolute_path const &install_root,
	                                         ventura::absolute_path const &cmake_exe, unsigned make_parallelism,
	                                         Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const module_in_cache = install_root / "cppnetlib";
		if (!ventura::file_exists(module_in_cache, Si::throw_))
		{
			ventura::absolute_path const &build_dir = temporary;
			ventura::create_directories(build_dir, Si::throw_);
			{
				std::vector<Si::noexcept_string> arguments;
				arguments.emplace_back("-DCMAKE_INSTALL_PREFIX=" + ventura::to_utf8_string(module_in_cache));
				arguments.emplace_back("-DCPP-NETLIB_BUILD_SHARED_LIBS=OFF");
				arguments.emplace_back("-DCPP-NETLIB_BUILD_TESTS=OFF");
				arguments.emplace_back("-DCPP-NETLIB_BUILD_EXPERIMENTS=OFF");
				arguments.emplace_back("-DCPP-NETLIB_BUILD_EXAMPLES=OFF");
#ifdef _WIN32
				// TODO: deal with OpenSSL later..
				arguments.emplace_back("-DCPP-NETLIB_ENABLE_HTTPS=OFF");
#endif
				ventura::absolute_path const boost_temp = temporary / "boost";
				ventura::create_directories(boost_temp, Si::throw_);
				cdm::boost_paths const boost_installed =
				    cdm::install_boost(boost_source, boost_temp, install_root, make_parallelism, output);
				cdm::generate_cmake_definitions_for_using_boost(Si::make_container_sink(arguments),
				                                                boost_installed.root);
				cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments));
				arguments.emplace_back(ventura::to_utf8_string(cppnetlib_source));
				int const rc = ventura::run_process(cmake_exe, arguments, build_dir, output,
				                                    std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
				                                    ventura::environment_inheritance::inherit).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake configure failed");
				}
			}
			{
#ifdef _MSC_VER
				boost::ignore_unused_variable_warning(make_parallelism);
				std::vector<Si::noexcept_string> arguments;
				arguments.emplace_back("CPP-NETLIB.sln");
				arguments.emplace_back("/build");
				arguments.emplace_back(
#ifdef NDEBUG
				    "Release"
#else
				    "Debug"
#endif
				    );
				arguments.emplace_back("/project");
				arguments.emplace_back("INSTALL");
				int const rc = ventura::run_process(
				                   *ventura::absolute_path::create(
#if _MSC_VER == 1900
				                       "C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\Common7\\IDE\\devenv.exe"
#elif _MSC_VER == 1800
				                       "C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\Common7\\IDE\\devenv.exe"
#else
#error unsupported version
#endif
				                       ),
				                   arguments, build_dir, output,
				                   std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
				                   ventura::environment_inheritance::inherit).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake build failed");
				}
#else
				std::vector<Si::noexcept_string> arguments;
				arguments.emplace_back("--build");
				arguments.emplace_back(".");
				arguments.emplace_back("--");
#ifdef _WIN32
				boost::ignore_unused_variable_warning(make_parallelism);
#else
				arguments.emplace_back("-j" + boost::lexical_cast<Si::noexcept_string>(make_parallelism));
#endif
				arguments.emplace_back("install");
				int const rc = ventura::run_process(cmake_exe, arguments, build_dir, output,
				                                    std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
				                                    ventura::environment_inheritance::inherit).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake build failed");
				}
#endif
			}
		}
		cppnetlib_paths result;
		result.cmake_prefix_path = module_in_cache /
#ifdef __linux__
		                           "lib/"
#if !CDM_TESTS_RUNNING_ON_TRAVIS_CI
		                           "x86_64-linux-gnu/"
#endif
		                           "cmake"
#else
		                           "CMake"
#endif
		    ;
		return result;
	}
}

#endif
