#define CDM_CONFIGURE_NAMESPACE a0038899
#include "../../cdm/application/using_catch/cdm.hpp"
#include "log.hpp"
#include <boost/test/unit_test.hpp>
#include <silicium/sink/ostream_sink.hpp>
#include <silicium/file_operations.hpp>
#include <cdm/locate_cache.hpp>

namespace
{
	Si::absolute_path const this_file = *Si::absolute_path::create(__FILE__);
	Si::absolute_path const test = *Si::parent(this_file);
	Si::absolute_path const repository = *Si::parent(test);
}

BOOST_AUTO_TEST_CASE(test_using_catch)
{
	Si::absolute_path const app_source = repository / Si::relative_path("application/using_catch");
	Si::absolute_path const tmp = Si::temporary_directory(Si::throw_) / *Si::path_segment::create("cdm");
	Si::absolute_path const module_temporaries = tmp / *Si::path_segment::create("mod");
	Si::absolute_path const application_build_dir = tmp / *Si::path_segment::create("app");
	Si::recreate_directories(module_temporaries, Si::throw_);
	Si::recreate_directories(application_build_dir, Si::throw_);
	auto output = cdm::make_program_output_printer(Si::ostream_ref_sink(std::cerr));
	unsigned const cpu_parallelism =
		#ifdef SILICIUM_TESTS_RUNNING_ON_TRAVIS_CI
				2;
		#else
				boost::thread::hardware_concurrency();
		#endif
	CDM_CONFIGURE_NAMESPACE::configure(module_temporaries, cdm::locate_cache(), app_source, application_build_dir, cpu_parallelism, output);
	{
		std::vector<Si::os_string> arguments;
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("--build"));
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("."));
		BOOST_REQUIRE_EQUAL(0, Si::run_process(Si::cmake_exe, arguments, application_build_dir, output));
	}
	{
		std::vector<Si::os_string> arguments;
		Si::relative_path const relative(
#ifdef _MSC_VER
			SILICIUM_SYSTEM_LITERAL("Debug/")
#endif
			SILICIUM_SYSTEM_LITERAL("using_catch")
#ifdef _MSC_VER
			SILICIUM_SYSTEM_LITERAL(".exe")
#endif
		);
		BOOST_REQUIRE_EQUAL(0, Si::run_process(application_build_dir / relative, arguments, application_build_dir, output));
	}
}
