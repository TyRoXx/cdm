#ifndef CDM_LIBGIT2_HPP
#define CDM_LIBGIT2_HPP

#include <ventura/file_operations.hpp>
#include <ventura/run_process.hpp>
#include <boost/lexical_cast.hpp>
#include <cdm/cmake_generator.hpp>
#include <silicium/sink/iterator_sink.hpp>

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
	                                     cdm::configuration const &target,
	                                     Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const module_in_cache = install_root / "libgit2";
		if (!ventura::file_exists(module_in_cache, Si::throw_))
		{
			ventura::absolute_path const &build_dir = temporary;
			ventura::create_directories(build_dir, Si::throw_);
			{
				std::vector<Si::noexcept_string> arguments;
				arguments.emplace_back("-DCMAKE_INSTALL_PREFIX=" + ventura::to_utf8_string(module_in_cache));
				arguments.emplace_back("-DBUILD_SHARED_LIBS=OFF");
				arguments.emplace_back("-DBUILD_CLAR=OFF");
#ifdef _MSC_VER
				arguments.emplace_back("-DSTATIC_CRT=OFF");
#endif
				cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments), target);
				arguments.emplace_back(ventura::to_utf8_string(original_source));
				int const rc = ventura::run_process(cmake_exe, arguments, build_dir, output,
				                                    std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
				                                    ventura::environment_inheritance::inherit)
				                   .get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake configure failed");
				}
			}
			{
				std::vector<Si::noexcept_string> arguments;
#ifdef _MSC_VER
				boost::ignore_unused_variable_warning(make_parallelism);
				arguments.emplace_back("libgit2.sln");
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
				                   ventura::environment_inheritance::inherit)
				                   .get();
#else
				cdm::generate_cmake_build_arguments(Si::make_container_sink(arguments), target);
				arguments.emplace_back("--");
				arguments.emplace_back("-j" + boost::lexical_cast<Si::noexcept_string>(make_parallelism));
				arguments.emplace_back("install");
				int const rc = ventura::run_process(cmake_exe, arguments, build_dir, output,
				                                    std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
				                                    ventura::environment_inheritance::inherit)
				                   .get();
#endif
				if (rc != 0)
				{
					throw std::runtime_error("cmake build failed");
				}
			}
		}
		libgit2_paths result;
		result.include = module_in_cache / "include";
		result.library_dir = module_in_cache / "lib";
		return result;
	}
}

#endif
