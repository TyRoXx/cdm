#include "cdm_libgit2.hpp"
#include <silicium/cmake.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	void configure(
		Si::absolute_path const &module_temporaries,
		Si::absolute_path const &module_permanent,
		Si::absolute_path const &application_source,
		Si::absolute_path const &application_build_dir,
		Si::optional<Si::absolute_path> const &boost_root,
		Si::Sink<char, Si::success>::interface &output
		)
	{
		boost::ignore_unused_variable_warning(boost_root);
		Si::optional<Si::absolute_path> const applications = Si::parent(application_source);
		if (!applications)
		{
			throw std::runtime_error("expected the source dir to have a parent");
		}
		Si::optional<Si::absolute_path> const cdm = Si::parent(*applications);
		if (!applications)
		{
			throw std::runtime_error("expected the applications dir to have a parent");
		}
		Si::absolute_path const original_source = *cdm / Si::relative_path("original_sources/libgit2-0.23.2");
		unsigned const parallelism =
#ifdef SILICIUM_TESTS_RUNNING_ON_TRAVIS_CI
			2;
#else
			boost::thread::hardware_concurrency();
#endif
		cdm::libgit2_paths const installed = cdm::install_libgit2(original_source, module_temporaries, module_permanent, Si::cmake_exe, parallelism, output);
		std::vector<Si::os_string> arguments;
		arguments.push_back(Si::os_string(SILICIUM_SYSTEM_LITERAL("-DLIBGIT2_INCLUDE_DIR=")) + to_os_string(installed.include));
		arguments.push_back(Si::os_string(SILICIUM_SYSTEM_LITERAL("-DLIBGIT2_LIBRARY_DIR=")) + to_os_string(installed.library_dir));
#ifdef _MSC_VER
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-G \"Visual Studio 12 2013\""));
#endif
		arguments.push_back(Si::to_os_string(application_source));
		if (Si::run_process(Si::cmake_exe, arguments, application_build_dir, output).get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}
	}
}