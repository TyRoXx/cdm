#ifndef CDM_SQLITE_HPP
#define CDM_SQLITE_HPP

#include <silicium/file_operations.hpp>
#include <silicium/run_process.hpp>
#include <silicium/write_file.hpp>
#include <silicium/sink/iterator_sink.hpp>
#include <boost/foreach.hpp>

namespace cdm
{
	struct sqlite_paths
	{
		Si::absolute_path include;
		Si::absolute_path library;
	};

	namespace sqlite3
	{
		inline Si::relative_path make_static_lib_build_path(Si::path_segment const &name_base)
		{
#ifdef _WIN32
			return Si::relative_path(L"Debug") / (name_base + *Si::path_segment::create(L".lib"));
#else
			return Si::relative_path("lib" + name_base.underlying() + ".a");
#endif
		}

		inline Si::relative_path make_static_lib_install_path(Si::path_segment const &name_base)
		{
#ifdef _WIN32
			return Si::relative_path(name_base + *Si::path_segment::create(L".lib"));
#else
			return Si::relative_path("lib" + name_base.underlying() + ".a");
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

	inline sqlite_paths install_sqlite(
		Si::absolute_path const &original_source,
		Si::absolute_path const &temporarily_writable,
		Si::absolute_path const &install_root,
		Si::absolute_path const &cmake_exe,
		Si::Sink<char, Si::success>::interface &output)
	{
		Si::absolute_path const in_cache = install_root / Si::relative_path("sqlite3");
		if (!Si::file_exists(in_cache, Si::throw_))
		{
			Si::absolute_path const build_dir = temporarily_writable / Si::relative_path("build");
			Si::create_directories(build_dir, Si::throw_);

			{
				std::vector<char> cmakeLists;
				auto writer = Si::make_container_sink(cmakeLists);
				Si::append(
					writer,
					"cmake_minimum_required(VERSION 2.8)\n"
					"project(sqlite3)\n"
					"add_library(sqlite3 "
				);
				{
					Si::absolute_path const sqlite3_c = original_source / Si::relative_path("sqlite3.c");
					encode_cmake_path_literal(Si::make_contiguous_range(Si::to_utf8_string(sqlite3_c)), writer);
				}
				Si::append(
					writer,
					")\n"
				);
				Si::throw_if_error(Si::write_file(Si::native_path_string((build_dir / Si::relative_path("CMakeLists.txt")).c_str()), Si::make_memory_range(cmakeLists)));
			}

			{
				std::vector<Si::os_string> arguments;
#ifdef _MSC_VER
				arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-G \"Visual Studio 12 2013\""));
#endif
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("."));
				int rc = Si::run_process(cmake_exe, arguments, build_dir, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake configure failed");
				}
			}
			{
				std::vector<Si::os_string> arguments;
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("--build"));
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("."));
				int rc = Si::run_process(cmake_exe, arguments, build_dir, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("cmake build failed");
				}
			}
			Si::absolute_path const construction_site = temporarily_writable / Si::relative_path("construction");
			Si::create_directories(construction_site, Si::throw_);
			{
				Si::absolute_path const lib_dir = construction_site / Si::relative_path("lib");
				Si::create_directories(lib_dir, Si::throw_);
				Si::copy(build_dir / sqlite3::make_static_lib_build_path(*Si::path_segment::create("sqlite3")), lib_dir / sqlite3::make_static_lib_install_path(*Si::path_segment::create(L"sqlite3")), Si::throw_);
				Si::absolute_path const include_dir = construction_site / Si::relative_path("include");
				Si::create_directories(include_dir, Si::throw_);
				Si::copy(original_source / *Si::path_segment::create("sqlite3.h"), include_dir / Si::relative_path("sqlite3.h"), Si::throw_);
				Si::copy(original_source / *Si::path_segment::create("sqlite3ext.h"), include_dir / Si::relative_path("sqlite3ext.h"), Si::throw_);
			}
			Si::rename(construction_site, in_cache, Si::throw_);
		}
		sqlite_paths result;
		result.include = in_cache / *Si::path_segment::create("include");
		result.library = in_cache / *Si::path_segment::create("lib") / sqlite3::make_static_lib_install_path(*Si::path_segment::create(L"sqlite3"));
		return std::move(result);
	}
}

#endif
