#define CDM_CONFIGURE_NAMESPACE a0038876
#include "../../cdm/application/using_boost/cdm.hpp"
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

#if !CDM_TESTS_RUNNING_ON_APPVEYOR
BOOST_AUTO_TEST_CASE(test_using_boost)
{
	cdm::travis_keep_alive_printer keep_travis_alive;
	ventura::absolute_path const app_source = repository / "application/using_boost";
	ventura::absolute_path const tmp = ventura::temporary_directory(Si::throw_) / "cdm_b";
	ventura::recreate_directories(tmp, Si::throw_);
	ventura::absolute_path const module_temporaries = tmp / "build";
	ventura::create_directories(module_temporaries, Si::throw_);
	ventura::absolute_path const application_build_dir = tmp / "app_build";
	ventura::create_directories(application_build_dir, Si::throw_);
	std::unique_ptr<std::ofstream> log_file = cdm::open_log(tmp / "test_using_boost.txt");
	auto output = cdm::make_program_output_printer(Si::ostream_ref_sink(*log_file));
	unsigned const cpu_parallelism =
#if CDM_TESTS_RUNNING_ON_TRAVIS_CI
	    2;
#else
	    boost::thread::hardware_concurrency();
#endif
	cdm::configure_result const configured = CDM_CONFIGURE_NAMESPACE::configure(
	    module_temporaries, cdm::locate_cache(), app_source, application_build_dir, cpu_parallelism, output);
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
		    SILICIUM_OS_STR("Debug/")
#endif
		        SILICIUM_OS_STR("using_boost")
#ifdef _MSC_VER
		            SILICIUM_OS_STR(".exe")
#endif
		                );
		BOOST_REQUIRE_EQUAL(
		    0, ventura::run_process(application_build_dir / relative, arguments, application_build_dir, output));
	}
}
#endif
