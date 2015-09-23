#include "../../cdm/building_configure/configure.hpp"
#include "log.hpp"
#include "boost_root.hpp"
#include <boost/test/unit_test.hpp>
#include <silicium/file_operations.hpp>
#include <silicium/sink/ostream_sink.hpp>
#include <cdm/locate_cache.hpp>

namespace
{
	Si::absolute_path const this_file = *Si::absolute_path::create(__FILE__);
	Si::absolute_path const test = *Si::parent(this_file);
	Si::absolute_path const repository = *Si::parent(test);
	Si::absolute_path const using_gtest_source = repository / Si::relative_path("application/using_gtest");
}

BOOST_AUTO_TEST_CASE(test_run_configure_command_line)
{
	Si::absolute_path const temporary_root = Si::temporary_directory(Si::throw_) / Si::relative_path("cdm_system_test");
	Si::recreate_directories(temporary_root, Si::throw_);
	Si::absolute_path const configure = temporary_root / Si::relative_path("configure");
	Si::absolute_path const &application = using_gtest_source;
	auto output = cdm::make_program_output_printer(Si::ostream_ref_sink(std::cerr));
	Si::absolute_path const application_build = temporary_root / Si::relative_path("application_build");
	Si::create_directories(application_build, Si::throw_);
	cdm::do_configure(temporary_root, cdm::locate_cache(), application, application_build, cdm::get_boost_root_for_testing(), output);
}
