#ifndef CDM_SQLITE_HPP
#define CDM_SQLITE_HPP

#include <ventura/file_operations.hpp>
#include <ventura/run_process.hpp>
#include <ventura/write_file.hpp>
#include <silicium/sink/iterator_sink.hpp>
#include <boost/foreach.hpp>

namespace cdm
{
	struct sqlite_paths
	{
		ventura::absolute_path include;
		ventura::absolute_path library;
	};

	namespace sqlite3
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

	template <class Sink>
	void encode_cmake_path_literal(Si::memory_range const &string, Sink &&out)
	{
		Si::append(out, '"');
		BOOST_FOREACH (char c, string)
		{
			switch (c)
			{
			case '"':
				Si::append(out, '\\');
				Si::append(out, c);
				break;

			case '\\':
				Si::append(out, '/');
				break;

			default:
				Si::append(out, c);
				break;
			}
		}
		Si::append(out, '"');
	}

	inline sqlite_paths install_sqlite(ventura::absolute_path const &original_source,
	                                   ventura::absolute_path const &temporarily_writable,
	                                   ventura::absolute_path const &install_root,
	                                   ventura::absolute_path const &cmake_exe,
	                                   Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const in_cache = install_root / "sqlite3";
		if (!ventura::file_exists(in_cache, Si::throw_))
		{
			ventura::absolute_path const build_dir = temporarily_writable / "build";
			ventura::create_directories(build_dir, Si::throw_);

			{
				std::vector<char> cmakeLists;
				auto writer = Si::make_container_sink(cmakeLists);
				Si::append(writer, "cmake_minimum_required(VERSION 2.8)\n"
				                   "project(sqlite3)\n"
				                   "add_library(sqlite3 ");
				{
					ventura::absolute_path const sqlite3_c = original_source / "sqlite3.c";
					encode_cmake_path_literal(Si::make_contiguous_range(to_utf8_string(sqlite3_c)), writer);
				}
				Si::append(writer, ")\n");
				Si::throw_if_error(
				    ventura::write_file(Si::native_path_string(to_os_string(build_dir / "CMakeLists.txt").c_str()),
				                        Si::make_memory_range(cmakeLists)));
			}

			{
				std::vector<Si::os_string> arguments;
#ifdef _MSC_VER
				arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-G \"Visual Studio 12 2013\""));
#endif
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("."));
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
			ventura::absolute_path const construction_site = temporarily_writable / "construction";
			ventura::create_directories(construction_site, Si::throw_);
			{
				ventura::absolute_path const lib_dir = construction_site / "lib";
				ventura::create_directories(lib_dir, Si::throw_);
				ventura::copy(
				    build_dir / sqlite3::make_static_lib_build_path(*ventura::path_segment::create("sqlite3")),
				    lib_dir / sqlite3::make_static_lib_install_path(*ventura::path_segment::create("sqlite3")),
				    Si::throw_);
				ventura::absolute_path const include_dir = construction_site / "include";
				ventura::create_directories(include_dir, Si::throw_);
				ventura::copy(original_source / "sqlite3.h", include_dir / "sqlite3.h", Si::throw_);
				ventura::copy(original_source / "sqlite3ext.h", include_dir / "sqlite3ext.h", Si::throw_);
			}
			ventura::rename(construction_site, in_cache, Si::throw_);
		}
		sqlite_paths result;
		result.include = in_cache / "include";
		result.library =
		    in_cache / "lib" / sqlite3::make_static_lib_install_path(*ventura::path_segment::create("sqlite3"));
		return std::move(result);
	}
}

#endif
