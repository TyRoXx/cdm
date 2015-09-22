#define CDM_CONFIGURE_NAMESPACE a0038876
#include "../../cdm/application/using_boost/cdm.hpp"
#include "log.hpp"
#include "boost_root.hpp"
#include <boost/test/unit_test.hpp>
#include <silicium/sink/ostream_sink.hpp>
#include <silicium/file_operations.hpp>

#ifdef _WIN32
#include <shlobj.h>
#else
#include <pwd.h>
#endif

namespace
{
	Si::absolute_path const this_file = *Si::absolute_path::create(__FILE__);
	Si::absolute_path const test = *Si::parent(this_file);
	Si::absolute_path const repository = *Si::parent(test);

#ifdef _WIN32
	struct CoTaskMemFreeDeleter
	{
		void operator()(void *memory) const
		{
			CoTaskMemFree(memory);
		}
	};

	Si::absolute_path get_home()
	{
		PWSTR path;
		HRESULT rc = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_CREATE, NULL, &path);
		if (rc != S_OK)
		{
			throw std::runtime_error("Could not get home");
		}
		std::unique_ptr<wchar_t, CoTaskMemFreeDeleter> raii_path(path);
		return Si::absolute_path::create(raii_path.get()).or_throw([] { throw std::runtime_error("Windows returned a non-absolute path for home"); });
	}
#else
	Si::absolute_path get_home()
	{
		return *Si::absolute_path::create(getpwuid(getuid())->pw_dir);
	}
#endif
}

BOOST_AUTO_TEST_CASE(test_using_boost)
{
	Si::absolute_path const app_source = repository / Si::relative_path("application/using_boost");
	Si::absolute_path const tmp = Si::temporary_directory(Si::throw_) / *Si::path_segment::create("cdm_b");
	Si::recreate_directories(tmp, Si::throw_);
	Si::absolute_path const module_temporaries = tmp / *Si::path_segment::create("build");
	Si::create_directories(module_temporaries, Si::throw_);
	Si::absolute_path const module_permanent = get_home() / Si::relative_path(".cdm_cache");
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
		BOOST_REQUIRE_EQUAL(0, Si::run_process(application_build_dir / relative, arguments, application_build_dir, output));
	}
}
