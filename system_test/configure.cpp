#include "../../cdm/building_configure/configure.hpp"
#include "log.hpp"
#include "temporary.hpp"
#include "boost_root.hpp"
#include <boost/test/unit_test.hpp>
#include <ventura/file_operations.hpp>
#include <silicium/sink/ostream_sink.hpp>
#include <cdm/locate_cache.hpp>

namespace
{
	ventura::absolute_path const this_file = *ventura::absolute_path::create(__FILE__);
	ventura::absolute_path const test = *ventura::parent(this_file);
	ventura::absolute_path const repository = *ventura::parent(test);
	ventura::absolute_path const using_gtest_source = repository / "application/using_gtest";
}

BOOST_AUTO_TEST_CASE(test_run_configure_command_line)
{
	ventura::absolute_path const temporary_root = cdm::get_temporary_root_for_testing() / "cdm_system_test";
	ventura::recreate_directories(temporary_root, Si::throw_);
	ventura::absolute_path const configure = temporary_root / "configure";
	ventura::absolute_path const &application = using_gtest_source;
	std::unique_ptr<std::ofstream> log_file = cdm::open_log(temporary_root / "test_run_configure_command_line.txt");
	auto output = cdm::make_program_output_printer(Si::ostream_ref_sink(*log_file));
	ventura::absolute_path const application_build = temporary_root / "application_build";
	ventura::create_directories(application_build, Si::throw_);
	cdm::do_configure(temporary_root, cdm::locate_cache_for_this_binary(), application, application_build,
	                  cdm::get_boost_root_for_testing(), cdm::approximate_configuration_of_this_binary(), output);
}
