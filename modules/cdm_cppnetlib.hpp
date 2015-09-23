#ifndef CDM_CPPNETLIB_HPP
#define CDM_CPPNETLIB_HPP

#include "cdm_boost.hpp"
#include <silicium/file_operations.hpp>
#include <silicium/run_process.hpp>
#include <boost/lexical_cast.hpp>

namespace cdm
{
	struct cppnetlib_paths
	{
		Si::absolute_path cmake_prefix_path;
	};

	inline cppnetlib_paths install_cppnetlib(
		Si::absolute_path const &cppnetlib_source,
		Si::absolute_path const &boost_source,
		Si::absolute_path const &temporary,
		Si::absolute_path const &install_root,
		Si::absolute_path const &cmake_exe,
		unsigned make_parallelism,
		Si::Sink<char, Si::success>::interface &output)
	{
		Si::absolute_path const module_in_cache = install_root / Si::relative_path("cppnetlib");
		if (!Si::file_exists(module_in_cache, Si::throw_))
		{
			Si::absolute_path const &build_dir = temporary;
			Si::create_directories(build_dir, Si::throw_);
			{
				std::vector<Si::os_string> arguments;
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DCMAKE_INSTALL_PREFIX=") + to_os_string(module_in_cache));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DCPP-NETLIB_BUILD_SHARED_LIBS=OFF"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DCPP-NETLIB_BUILD_TESTS=OFF"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DCPP-NETLIB_BUILD_EXPERIMENTS=OFF"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DCPP-NETLIB_BUILD_EXAMPLES=OFF"));
#ifdef _WIN32
				//TODO: deal with OpenSSL later..
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DCPP-NETLIB_ENABLE_HTTPS=OFF"));
#endif
				Si::absolute_path const boost_temp = temporary / Si::relative_path("boost");
				cdm::boost_paths const boost_installed = cdm::install_boost(boost_source, boost_temp, install_root, cmake_exe, make_parallelism, output);
				arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBOOST_ROOT=") + Si::to_os_string(boost_installed.root));
				arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBoost_ADDITIONAL_VERSIONS=1.59"));
#ifdef _MSC_VER
				arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-G \"Visual Studio 12 2013\""));
#endif
				arguments.push_back(cppnetlib_source.c_str());
				int const rc = Si::run_process(cmake_exe, arguments, build_dir, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake configure failed");
				}
			}
			{
#ifdef _MSC_VER
				boost::ignore_unused_variable_warning(make_parallelism);
				std::vector<Si::os_string> arguments;
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("CPP-NETLIB.sln"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("/build"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("Debug"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("/project"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("INSTALL"));
				int const rc = Si::run_process(
					*Si::absolute_path::create(L"C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\Common7\\IDE\\devenv.exe"),
					arguments, build_dir, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake build failed");
				}
#else
				std::vector<Si::os_string> arguments;
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("--build"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("."));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("--"));
#ifndef _WIN32
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-j" + boost::lexical_cast<Si::os_string>(make_parallelism)));
#else
				boost::ignore_unused_variable_warning(make_parallelism);
#endif
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("install"));
				int const rc = Si::run_process(cmake_exe, arguments, build_dir, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake build failed");
				}
#endif
			}
		}
		cppnetlib_paths result;
		result.cmake_prefix_path = module_in_cache / Si::relative_path(
#ifdef __linux__
			"lib/"
#if !defined(CDM_TESTS_RUNNING_ON_TRAVIS_CI)
			"x86_64-linux-gnu/"
#endif
			"cmake"
#else
			"CMake"
#endif
		);
		return result;
	}
}

#endif
