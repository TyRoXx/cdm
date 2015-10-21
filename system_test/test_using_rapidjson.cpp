#define CDM_CONFIGURE_NAMESPACE a0038886
#include "../../cdm/application/using_rapidjson/cdm.hpp"
#include "log.hpp"
#include <boost/test/unit_test.hpp>
#include <silicium/sink/ostream_sink.hpp>
#include <ventura/file_operations.hpp>
#include <cdm/locate_cache.hpp>

namespace
{
	ventura::absolute_path const this_file = *ventura::absolute_path::create(__FILE__);
	ventura::absolute_path const test = *ventura::parent(this_file);
	ventura::absolute_path const repository = *ventura::parent(test);
}

BOOST_AUTO_TEST_CASE(test_using_rapidjson)
{
	ventura::absolute_path const app_source = repository / ventura::relative_path("application/using_rapidjson");
	ventura::absolute_path const tmp = ventura::temporary_directory(Si::throw_) / *ventura::path_segment::create("cdm_test_using_rapidjson");
	ventura::absolute_path const module_temporaries = tmp / *ventura::path_segment::create("module_temporaries");
	ventura::absolute_path const application_build_dir = tmp / *ventura::path_segment::create("application_build_dir");
	ventura::recreate_directories(module_temporaries, Si::throw_);
	ventura::recreate_directories(application_build_dir, Si::throw_);
	std::unique_ptr<std::ofstream> log_file = cdm::open_log(tmp / ventura::relative_path("test_using_rapidjson.txt"));
	auto output = cdm::make_program_output_printer(Si::ostream_ref_sink(*log_file));
	unsigned const cpu_parallelism =
#if CDM_TESTS_RUNNING_ON_TRAVIS_CI
	    2;
#else
	    boost::thread::hardware_concurrency();
#endif
	CDM_CONFIGURE_NAMESPACE::configure(module_temporaries, cdm::locate_cache(), app_source, application_build_dir, cpu_parallelism, output);
	{
		std::vector<Si::os_string> arguments;
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("--build"));
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("."));
		BOOST_REQUIRE_EQUAL(0, ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output));
	}
	{
		std::vector<Si::os_string> arguments;
		ventura::relative_path const relative(
#ifdef _MSC_VER
		    SILICIUM_SYSTEM_LITERAL("Debug/")
#endif
		        SILICIUM_SYSTEM_LITERAL("using_rapidjson")
#ifdef _MSC_VER
		            SILICIUM_SYSTEM_LITERAL(".exe")
#endif
		                );
		BOOST_REQUIRE_EQUAL(0, ventura::run_process(application_build_dir / relative, arguments, application_build_dir, output));
	}
}
