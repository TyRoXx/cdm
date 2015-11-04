#ifndef CDM_CPPNETLIB_HPP
#define CDM_CPPNETLIB_HPP

#include "cdm_boost.hpp"
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
				std::vector<Si::os_string> arguments;
				arguments.emplace_back(SILICIUM_OS_STR("-DCMAKE_INSTALL_PREFIX=") + to_os_string(module_in_cache));
				arguments.emplace_back(SILICIUM_OS_STR("-DCPP-NETLIB_BUILD_SHARED_LIBS=OFF"));
				arguments.emplace_back(SILICIUM_OS_STR("-DCPP-NETLIB_BUILD_TESTS=OFF"));
				arguments.emplace_back(SILICIUM_OS_STR("-DCPP-NETLIB_BUILD_EXPERIMENTS=OFF"));
				arguments.emplace_back(SILICIUM_OS_STR("-DCPP-NETLIB_BUILD_EXAMPLES=OFF"));
#ifdef _WIN32
				// TODO: deal with OpenSSL later..
				arguments.emplace_back(SILICIUM_OS_STR("-DCPP-NETLIB_ENABLE_HTTPS=OFF"));
#endif
				ventura::absolute_path const boost_temp = temporary / "boost";
				ventura::create_directories(boost_temp, Si::throw_);
				cdm::boost_paths const boost_installed =
				    cdm::install_boost(boost_source, boost_temp, install_root, make_parallelism, output);
				arguments.emplace_back(SILICIUM_OS_STR("-DBOOST_ROOT=") + to_os_string(boost_installed.root));
#if CDM_TESTS_RUNNING_ON_APPVEYOR
				arguments.emplace_back(SILICIUM_OS_STR("-DBOOST_LIBRARYDIR=") +
				                       to_os_string(boost_installed.root / "lib32-msvc-14.0"));
#endif
				arguments.emplace_back(SILICIUM_OS_STR("-DBoost_ADDITIONAL_VERSIONS=1.59"));
				arguments.emplace_back(SILICIUM_OS_STR("-DBoost_NO_SYSTEM_PATHS=ON"));
#ifdef _MSC_VER
				arguments.emplace_back(SILICIUM_OS_STR("-G \"Visual Studio 12 2013\""));
#endif
				arguments.emplace_back(to_os_string(cppnetlib_source));
				int const rc = ventura::run_process(cmake_exe, arguments, build_dir, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake configure failed");
				}
			}
			{
#ifdef _MSC_VER
				boost::ignore_unused_variable_warning(make_parallelism);
				std::vector<Si::os_string> arguments;
				arguments.emplace_back(SILICIUM_OS_STR("CPP-NETLIB.sln"));
				arguments.emplace_back(SILICIUM_OS_STR("/build"));
				arguments.emplace_back(SILICIUM_OS_STR("Debug"));
				arguments.emplace_back(SILICIUM_OS_STR("/project"));
				arguments.emplace_back(SILICIUM_OS_STR("INSTALL"));
				int const rc =
				    ventura::run_process(*ventura::absolute_path::create("C:\\Program Files (x86)\\Microsoft Visual "
				                                                         "Studio 12.0\\Common7\\IDE\\devenv.exe"),
				                         arguments, build_dir, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake build failed");
				}
#else
				std::vector<Si::os_string> arguments;
				arguments.emplace_back(SILICIUM_OS_STR("--build"));
				arguments.emplace_back(SILICIUM_OS_STR("."));
				arguments.emplace_back(SILICIUM_OS_STR("--"));
#ifdef _WIN32
				boost::ignore_unused_variable_warning(make_parallelism);
#else
				arguments.emplace_back(SILICIUM_OS_STR("-j" + boost::lexical_cast<Si::os_string>(make_parallelism)));
#endif
				arguments.emplace_back(SILICIUM_OS_STR("install"));
				int const rc = ventura::run_process(cmake_exe, arguments, build_dir, output).get();
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
