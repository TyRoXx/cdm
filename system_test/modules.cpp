#include <cdm_cppnetlib.hpp>
#include <cdm_gtest.hpp>
#include <silicium/cmake.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/thread/thread.hpp>
#include "log.hpp"
#include <silicium/sink/ostream_sink.hpp>
#include <cdm/locate_cache.hpp>

namespace
{
	Si::absolute_path const this_file = *Si::absolute_path::create(__FILE__);
	Si::absolute_path const test = *Si::parent(this_file);
	Si::absolute_path const repository = *Si::parent(test);
}

BOOST_AUTO_TEST_CASE(test_cdm_gtest)
{
	Si::absolute_path const source = repository / Si::relative_path("original_sources/gtest-1.7.0");
	Si::absolute_path const tmp = Si::temporary_directory(Si::throw_) / Si::relative_path("cdm");
	Si::absolute_path const build_dir = tmp / *Si::path_segment::create("build");
	Si::recreate_directories(build_dir, Si::throw_);
	auto output = cdm::make_program_output_printer(Si::ostream_ref_sink(std::cerr));
	Si::absolute_path const modules = cdm::locate_cache();
	cdm::gtest_paths const built = cdm::install_gtest(source, build_dir, modules, Si::cmake_exe, output);
	BOOST_CHECK_EQUAL(modules / *Si::path_segment::create("gtest") / *Si::path_segment::create("include"), built.include);
	BOOST_CHECK(boost::filesystem::exists(built.include.to_boost_path()));
	BOOST_CHECK(boost::filesystem::exists(built.library.to_boost_path()));
	BOOST_CHECK(boost::filesystem::exists(built.library_main.to_boost_path()));
}

BOOST_AUTO_TEST_CASE(test_cdm_cppnetlib)
{
	Si::absolute_path const source = repository / Si::relative_path("original_sources/cpp-netlib-0.11.2-final");
	Si::absolute_path const tmp = Si::temporary_directory(Si::throw_) / Si::relative_path("test_cdm_cppnetlib");
	unsigned const make_parallelism =
#ifdef SILICIUM_TESTS_RUNNING_ON_TRAVIS_CI
		2;
#else
		boost::thread::hardware_concurrency();
#endif
	Si::absolute_path const build_dir = tmp / *Si::path_segment::create("build");
	Si::recreate_directories(build_dir, Si::throw_);
	auto output = cdm::make_program_output_printer(Si::ostream_ref_sink(std::cerr));
	Si::absolute_path const boost_source = repository / Si::relative_path("original_sources/boost_1_59_0");
	cdm::cppnetlib_paths const built = cdm::install_cppnetlib(source, boost_source, build_dir, cdm::locate_cache(), Si::cmake_exe, make_parallelism, output);
	Si::absolute_path const cmake_directory = built.cmake_prefix_path
#ifndef _MSC_VER
		/ Si::relative_path("cppnetlib")
#endif
		;
	BOOST_CHECK(Si::file_exists(cmake_directory / Si::relative_path("cppnetlibConfig.cmake"), Si::throw_));
	BOOST_CHECK(Si::file_exists(cmake_directory / Si::relative_path("cppnetlibTargets.cmake"), Si::throw_));
	BOOST_CHECK(Si::file_exists(cmake_directory / Si::relative_path("cppnetlibConfigVersion.cmake"), Si::throw_));
	Si::relative_path const targets(
#ifdef _MSC_VER
		"cppnetlibTargets-debug.cmake"
#else
		"cppnetlibTargets-noconfig.cmake"
#endif
		);
	BOOST_CHECK(Si::file_exists(cmake_directory / targets, Si::throw_));
}
