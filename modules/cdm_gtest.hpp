#ifndef CDM_GTEST_HPP
#define CDM_GTEST_HPP

#include <ventura/file_operations.hpp>
#include <ventura/run_process.hpp>
#include <silicium/sink/iterator_sink.hpp>
#include <cdm/cmake_generator.hpp>

namespace cdm
{
	struct gtest_paths
	{
		ventura::absolute_path include;
		ventura::absolute_path library;
		ventura::absolute_path library_main;
	};

	namespace gtest
	{
		inline ventura::relative_path make_static_lib_build_path(ventura::path_segment const &name_base)
		{
#ifdef _WIN32
			return ventura::relative_path("Debug") / (name_base + *ventura::path_segment::create(".lib"));
#else
			return ventura::relative_path("lib" + name_base.underlying() + ".a");
#endif
		}

		inline ventura::relative_path make_static_lib_install_path(ventura::path_segment const &name_base)
		{
#ifdef _WIN32
			return ventura::relative_path(name_base + *ventura::path_segment::create(".lib"));
#else
			return ventura::relative_path("lib" + name_base.underlying() + ".a");
#endif
		}
	}

	inline gtest_paths install_gtest(ventura::absolute_path const &gtest_source,
	                                 ventura::absolute_path const &temporarily_writable,
	                                 ventura::absolute_path const &install_root,
	                                 ventura::absolute_path const &cmake_exe, cdm::configuration const &target,
	                                 Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const gtest_in_cache = install_root / "gtest";
		if (!ventura::file_exists(gtest_in_cache, Si::throw_))
		{
			ventura::absolute_path const build_dir = temporarily_writable / "build";
			ventura::create_directories(build_dir, Si::throw_);
			{
				std::vector<Si::noexcept_string> arguments;
#ifdef _MSC_VER
				arguments.emplace_back("-DBUILD_SHARED_LIBS=OFF");
				arguments.emplace_back("-Dgtest_force_shared_crt=ON");
#endif
				cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments), target);
				arguments.emplace_back(ventura::to_utf8_string(gtest_source));
				int rc = ventura::run_process(cmake_exe, arguments, build_dir, output,
				                              std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
				                              ventura::environment_inheritance::inherit).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake configure failed");
				}
			}
			{
				std::vector<Si::noexcept_string> arguments;
				arguments.emplace_back("--build");
				arguments.emplace_back(".");
				int rc = ventura::run_process(cmake_exe, arguments, build_dir, output,
				                              std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
				                              ventura::environment_inheritance::inherit).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake build failed");
				}
			}
			ventura::absolute_path const construction_site = temporarily_writable / "construction";
			ventura::create_directories(construction_site, Si::throw_);
			{
				ventura::absolute_path const lib_dir = construction_site / "lib";
				ventura::create_directories(lib_dir, Si::throw_);
				ventura::copy(build_dir / gtest::make_static_lib_build_path(*ventura::path_segment::create("gtest")),
				              lib_dir / gtest::make_static_lib_install_path(*ventura::path_segment::create("gtest")),
				              Si::throw_);
				ventura::copy(
				    build_dir / gtest::make_static_lib_build_path(*ventura::path_segment::create("gtest_main")),
				    lib_dir / gtest::make_static_lib_install_path(*ventura::path_segment::create("gtest_main")),
				    Si::throw_);
				ventura::copy_recursively(gtest_source / "include", construction_site / "include", &output, Si::throw_);
			}
			ventura::create_directories(install_root, Si::throw_);
			ventura::rename(construction_site, gtest_in_cache, Si::throw_);
		}
		gtest_paths result;
		result.include = gtest_in_cache / "include";
		auto lib_dir = gtest_in_cache / "lib";
		result.library = lib_dir / gtest::make_static_lib_install_path(*ventura::path_segment::create("gtest"));
		result.library_main =
		    lib_dir / gtest::make_static_lib_install_path(*ventura::path_segment::create("gtest_main"));
		return result;
	}
}

#endif
