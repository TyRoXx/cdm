#ifndef CDM_GTEST_HPP
#define CDM_GTEST_HPP

#include <ventura/file_operations.hpp>
#include <ventura/run_process.hpp>

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
			return ventura::relative_path(L"Debug") / (name_base + *ventura::path_segment::create(L".lib"));
#else
			return ventura::relative_path("lib" + name_base.underlying() + ".a");
#endif
		}

		inline ventura::relative_path make_static_lib_install_path(ventura::path_segment const &name_base)
		{
#ifdef _WIN32
			return ventura::relative_path(name_base + *ventura::path_segment::create(L".lib"));
#else
			return ventura::relative_path("lib" + name_base.underlying() + ".a");
#endif
		}
	}

	inline gtest_paths install_gtest(ventura::absolute_path const &gtest_source, ventura::absolute_path const &temporarily_writable,
	                                 ventura::absolute_path const &install_root, ventura::absolute_path const &cmake_exe,
	                                 Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const gtest_in_cache = install_root / ventura::relative_path("gtest");
		if (!ventura::file_exists(gtest_in_cache, Si::throw_))
		{
			ventura::absolute_path const build_dir = temporarily_writable / ventura::relative_path("build");
			ventura::create_directories(build_dir, Si::throw_);
			{
				std::vector<Si::os_string> arguments;
#ifdef _MSC_VER
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DBUILD_SHARED_LIBS=OFF"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-Dgtest_force_shared_crt=ON"));
				arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-G \"Visual Studio 12 2013\""));
#endif
				arguments.push_back(gtest_source.c_str());
				int rc = ventura::run_process(cmake_exe, arguments, build_dir, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake configure failed");
				}
			}
			{
				std::vector<Si::os_string> arguments;
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("--build"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("."));
				int rc = ventura::run_process(cmake_exe, arguments, build_dir, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake build failed");
				}
			}
			ventura::absolute_path const construction_site = temporarily_writable / ventura::relative_path("construction");
			ventura::create_directories(construction_site, Si::throw_);
			{
				ventura::absolute_path const lib_dir = construction_site / ventura::relative_path("lib");
				ventura::create_directories(lib_dir, Si::throw_);
				ventura::copy(build_dir / gtest::make_static_lib_build_path(*ventura::path_segment::create("gtest")),
				              lib_dir / gtest::make_static_lib_install_path(*ventura::path_segment::create(L"gtest")), Si::throw_);
				ventura::copy(build_dir / gtest::make_static_lib_build_path(*ventura::path_segment::create("gtest_main")),
				              lib_dir / gtest::make_static_lib_install_path(*ventura::path_segment::create(L"gtest_main")), Si::throw_);
				ventura::copy_recursively(gtest_source / *ventura::path_segment::create("include"), construction_site / ventura::relative_path("include"),
				                          &output, Si::throw_);
			}
			ventura::rename(construction_site, gtest_in_cache, Si::throw_);
		}
		gtest_paths result;
		result.include = gtest_in_cache / *ventura::path_segment::create("include");
		auto lib_dir = gtest_in_cache / *ventura::path_segment::create("lib");
		result.library = lib_dir / gtest::make_static_lib_install_path(*ventura::path_segment::create(L"gtest"));
		result.library_main = lib_dir / gtest::make_static_lib_install_path(*ventura::path_segment::create(L"gtest_main"));
		return result;
	}
}

#endif
