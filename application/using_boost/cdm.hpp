#include "cdm_boost.hpp"
#include <silicium/cmake.hpp>
#include <silicium/run_process.hpp>
#include <cdm/configure_result.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	cdm::configure_result configure(
		Si::absolute_path const &module_temporaries,
		Si::absolute_path const &module_permanent,
		Si::absolute_path const &application_source,
		Si::absolute_path const &application_build_dir,
		unsigned cpu_parallelism,
		Si::Sink<char, Si::success>::interface &output
		)
	{
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
		Si::absolute_path const boost_source = *cdm / Si::relative_path("original_sources/boost_1_59_0");
		cdm::boost_paths const boost_installed = cdm::install_boost(boost_source, module_temporaries, module_permanent, cpu_parallelism, output);
		std::vector<Si::os_string> arguments;
		Si::os_string const our_boost_root = to_os_string(boost_installed.root);
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBOOST_ROOT=") + our_boost_root);
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBoost_ADDITIONAL_VERSIONS=1.59"));
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBoost_NO_SYSTEM_PATHS=ON"));
#ifdef _MSC_VER
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-G \"Visual Studio 12 2013\""));
#endif
		arguments.push_back(Si::to_os_string(application_source));
		if (Si::run_process(Si::cmake_exe, arguments, application_build_dir, output).get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}

		cdm::configure_result result;
		result.shared_library_directories.emplace_back(boost_installed.root / Si::relative_path("lib"));
		return result;
	}
}
