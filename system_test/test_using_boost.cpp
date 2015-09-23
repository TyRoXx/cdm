#define CDM_CONFIGURE_NAMESPACE a0038876
#include "../../cdm/application/using_boost/cdm.hpp"
#include "log.hpp"
#include "boost_root.hpp"
#include <boost/test/unit_test.hpp>
#include <silicium/sink/ostream_sink.hpp>
#include <silicium/file_operations.hpp>

namespace
{
	Si::absolute_path const this_file = *Si::absolute_path::create(__FILE__);
	Si::absolute_path const test = *Si::parent(this_file);
	Si::absolute_path const repository = *Si::parent(test);
}

BOOST_AUTO_TEST_CASE(test_using_boost)
{
	Si::absolute_path const app_source = repository / Si::relative_path("application/using_boost");
	Si::absolute_path const tmp = Si::temporary_directory(Si::throw_) / *Si::path_segment::create("cdm_b");
	Si::recreate_directories(tmp, Si::throw_);
	Si::absolute_path const module_temporaries = tmp / *Si::path_segment::create("build");
	Si::create_directories(module_temporaries, Si::throw_);
	Si::absolute_path const module_permanent = Si::get_home() / Si::relative_path(".cdm_cache");
	Si::absolute_path const application_build_dir = tmp / *Si::path_segment::create("app_build");
	Si::create_directories(application_build_dir, Si::throw_);
	auto output = cdm::make_program_output_printer(Si::ostream_ref_sink(std::cerr));
	CDM_CONFIGURE_NAMESPACE::configure(module_temporaries, module_permanent, app_source, application_build_dir, cdm::get_boost_root_for_testing(), output);
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
			SILICIUM_SYSTEM_LITERAL("using_boost")
#ifdef _MSC_VER
			SILICIUM_SYSTEM_LITERAL(".exe")
#endif
		);
		std::vector<std::pair<Si::os_char const *, Si::os_char const *>> environment;
		BOOST_REQUIRE_EQUAL(0, Si::run_process(
			application_build_dir / relative,
			arguments,
			application_build_dir,
			output,
			environment,
			Si::environment_inheritance::inherit
		));
	}
}
