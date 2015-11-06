#define CDM_CONFIGURE_NAMESPACE a0038877
#include "../../cdm/application/using_websocketpp/cdm.hpp"
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

BOOST_AUTO_TEST_CASE(test_using_websocketpp)
{
	cdm::travis_keep_alive_printer keep_travis_alive;
	ventura::absolute_path const app_source = repository / "application/using_websocketpp";
	ventura::absolute_path const tmp = ventura::temporary_directory(Si::throw_) / "cdm_test_using_websocketpp";
	ventura::absolute_path const module_temporaries = tmp / "module_temporaries";
	ventura::absolute_path const application_build_dir = tmp / "using_websocketpp";
	ventura::recreate_directories(module_temporaries, Si::throw_);
	ventura::recreate_directories(application_build_dir, Si::throw_);
	std::unique_ptr<std::ofstream> log_file = cdm::open_log(tmp / "test_using_websocketpp.txt");
	auto output = cdm::make_program_output_printer(Si::ostream_ref_sink(*log_file));
	unsigned const cpu_parallelism =
#if CDM_TESTS_RUNNING_ON_TRAVIS_CI
	    2;
#else
	    boost::thread::hardware_concurrency();
#endif
	CDM_CONFIGURE_NAMESPACE::configure(module_temporaries, cdm::locate_cache_for_this_binary(), app_source,
	                                   application_build_dir, cpu_parallelism, output);
	{
		std::vector<Si::os_string> arguments;
		arguments.emplace_back(SILICIUM_OS_STR("--build"));
		arguments.emplace_back(SILICIUM_OS_STR("."));
		BOOST_REQUIRE_EQUAL(0, ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output));
	}
	{
		std::vector<Si::os_string> arguments;
		ventura::relative_path const relative(
#ifdef _MSC_VER
		    "Debug/"
#endif
		    "using_websocketpp"
#ifdef _MSC_VER
		    ".exe"
#endif
		    );
		BOOST_REQUIRE_EQUAL(
		    0, ventura::run_process(application_build_dir / relative, arguments, application_build_dir, output));
	}
}
