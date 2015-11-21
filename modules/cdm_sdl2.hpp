#ifndef CDM_SDL2_HPP
#define CDM_SDL2_HPP

#include <cdm/cmake_generator.hpp>
#include <ventura/file_operations.hpp>
#include <ventura/run_process.hpp>
#include <boost/lexical_cast.hpp>
#include <silicium/sink/iterator_sink.hpp>

namespace cdm
{
	struct sdl2_paths
	{
		ventura::absolute_path include;
		ventura::absolute_path library;
		ventura::absolute_path main;
		ventura::absolute_path runtime_library;
	};

	inline sdl2_paths install_sdl2(ventura::absolute_path const &sdl2_source, ventura::absolute_path const &temporary,
	                               ventura::absolute_path const &install_root, ventura::absolute_path const &cmake_exe,
	                               unsigned make_parallelism, cdm::configuration const &target,
	                               Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const module_in_cache = install_root / "sdl2";
		if (!ventura::file_exists(module_in_cache, Si::throw_))
		{
			ventura::absolute_path const &build_dir = temporary / "build";
			ventura::absolute_path const &construction_site = temporary / "construction";
			ventura::create_directories(build_dir, Si::throw_);
			{
				std::vector<Si::noexcept_string> arguments;
				arguments.emplace_back("-DCMAKE_INSTALL_PREFIX=" + ventura::to_utf8_string(construction_site));
				arguments.emplace_back("-DSDL_STATIC=OFF");
				arguments.emplace_back("-DSDL_SHARED=ON");
				cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments), target);
				arguments.emplace_back(ventura::to_utf8_string(sdl2_source));
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
#ifdef _MSC_VER
				boost::ignore_unused_variable_warning(make_parallelism);
				std::vector<Si::os_string> arguments;
				arguments.emplace_back(SILICIUM_OS_STR("SDL2.sln"));
				arguments.emplace_back(SILICIUM_OS_STR("/build"));
				arguments.emplace_back(
#ifdef NDEBUG
				    SILICIUM_OS_STR("Release")
#else
				    SILICIUM_OS_STR("Debug")
#endif
				        );
				arguments.emplace_back(SILICIUM_OS_STR("/project"));
				arguments.emplace_back(SILICIUM_OS_STR("INSTALL"));
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
				if (rc != 0)
				{
					throw std::runtime_error("cmake build failed");
				}
#else
				std::vector<Si::noexcept_string> arguments;
				cdm::generate_cmake_build_arguments(Si::make_container_sink(arguments), target);
				arguments.emplace_back("--");
#ifdef _WIN32
				boost::ignore_unused_variable_warning(make_parallelism);
#else
				arguments.emplace_back("-j" + boost::lexical_cast<Si::os_string>(make_parallelism));
#endif
				arguments.emplace_back("install");
				int const rc = ventura::run_process(cmake_exe, arguments, build_dir, output,
				                                    std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
				                                    ventura::environment_inheritance::inherit)
				                   .get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake build failed");
				}
#endif
			}
			ventura::rename(construction_site, module_in_cache, Si::throw_);
		}
		sdl2_paths result;
		result.include = module_in_cache / "include/SDL2";
		auto make_library_name = [](ventura::path::underlying_type const &stem, bool is_shared_object)
		{
#ifdef _MSC_VER
			Si::ignore_unused_variable_warning(is_shared_object);
#endif
			// clang-format 3.7 mis-formats the following expression.
			// clang-format off
			auto result =
#ifndef _MSC_VER
					"lib" +
#endif
					stem +
#ifdef _MSC_VER
					".lib"
#else
					(is_shared_object ? ".so" : ".a")
#endif
					;
			// clang-format on
			return *ventura::path_segment::create(ventura::path(std::move(result)));
		};
		auto lib_dir = module_in_cache / "lib";
		result.library = lib_dir / make_library_name("SDL2", true);
		result.main = lib_dir / make_library_name("SDL2main", false);
		result.runtime_library =
#ifdef _MSC_VER
		    module_in_cache / "bin" /
#ifndef _MSC_VER
		    "lib"
#endif
		    "SDL2."
#ifdef _MSC_VER
		    "dll"
#else
		    "so"
#endif
#else
		    result.library
#endif
		    ;
		auto require_file = [](ventura::absolute_path const &file)
		{
			if (!ventura::file_exists(file, Si::throw_))
			{
				throw std::runtime_error("Missing from SDL2 installation: " +
				                         ventura::to_utf8_string<std::string>(file));
			}
		};
		require_file(result.library);
		require_file(result.main);
		require_file(result.runtime_library);
		return result;
	}
}

#endif
