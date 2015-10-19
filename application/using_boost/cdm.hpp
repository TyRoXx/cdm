#include "cdm_boost.hpp"
#include <ventura/cmake.hpp>
#include <ventura/run_process.hpp>
#include <cdm/configure_result.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	cdm::configure_result configure(ventura::absolute_path const &module_temporaries, ventura::absolute_path const &module_permanent,
	                                ventura::absolute_path const &application_source, ventura::absolute_path const &application_build_dir,
	                                unsigned cpu_parallelism, Si::Sink<char, Si::success>::interface &output)
	{
		Si::optional<ventura::absolute_path> const applications = ventura::parent(application_source);
		if (!applications)
		{
			throw std::runtime_error("expected the source dir to have a parent");
		}
		Si::optional<ventura::absolute_path> const cdm = ventura::parent(*applications);
		if (!applications)
		{
			throw std::runtime_error("expected the applications dir to have a parent");
		}
		ventura::absolute_path const boost_source = *cdm / ventura::relative_path("original_sources/boost_1_59_0");
		cdm::boost_paths const boost_installed = cdm::install_boost(boost_source, module_temporaries, module_permanent, cpu_parallelism, output);
		std::vector<Si::os_string> arguments;
		Si::os_string const our_boost_root = to_os_string(boost_installed.root);
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBOOST_ROOT=") + our_boost_root);
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBoost_ADDITIONAL_VERSIONS=1.59"));
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBoost_NO_SYSTEM_PATHS=ON"));
#ifdef _MSC_VER
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-G \"Visual Studio 12 2013\""));
#endif
		arguments.push_back(to_os_string(application_source));
		if (ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output).get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}

		cdm::configure_result result;
		result.shared_library_directories.emplace_back(boost_installed.root / ventura::relative_path("lib"));
		return result;
	}
}
