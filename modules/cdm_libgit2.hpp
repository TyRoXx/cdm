#ifndef CDM_LIBGIT2_HPP
#define CDM_LIBGIT2_HPP

#include <ventura/file_operations.hpp>
#include <ventura/run_process.hpp>
#include <boost/lexical_cast.hpp>

namespace cdm
{
	struct libgit2_paths
	{
		ventura::absolute_path include;
		ventura::absolute_path library_dir;
	};

	inline libgit2_paths install_libgit2(ventura::absolute_path const &original_source,
	                                     ventura::absolute_path const &temporary,
	                                     ventura::absolute_path const &install_root,
	                                     ventura::absolute_path const &cmake_exe, unsigned make_parallelism,
	                                     Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const module_in_cache = install_root / "libgit2";
		if (!ventura::file_exists(module_in_cache, Si::throw_))
		{
			ventura::absolute_path const &build_dir = temporary;
			ventura::create_directories(build_dir, Si::throw_);
			{
				std::vector<Si::os_string> arguments;
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DCMAKE_INSTALL_PREFIX=") + to_os_string(module_in_cache));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DBUILD_SHARED_LIBS=OFF"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DBUILD_CLAR=OFF"));
#ifdef _MSC_VER
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DSTATIC_CRT=OFF"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-G \"Visual Studio 12 2013\""));
#endif
				arguments.push_back(to_os_string(original_source));
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
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("libgit2.sln"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("/build"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("Debug"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("/project"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("INSTALL"));
				int const rc =
				    ventura::run_process(*ventura::absolute_path::create("C:\\Program Files (x86)\\Microsoft Visual "
				                                                         "Studio 12.0\\Common7\\IDE\\devenv.exe"),
				                         arguments, build_dir, output)
				        .get();
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
				arguments.push_back(
				    SILICIUM_SYSTEM_LITERAL("-j" + boost::lexical_cast<Si::os_string>(make_parallelism)));
#else
				boost::ignore_unused_variable_warning(make_parallelism);
#endif
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("install"));
				int const rc = ventura::run_process(cmake_exe, arguments, build_dir, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake build failed");
				}
#endif
			}
		}
		libgit2_paths result;
		result.include = module_in_cache / "include";
		result.library_dir = module_in_cache / "lib";
		return result;
	}
}

#endif
