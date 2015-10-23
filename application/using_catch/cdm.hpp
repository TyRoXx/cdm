#include "cdm_catch.hpp"
#include <ventura/cmake.hpp>
#include <cdm/configure_result.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	cdm::configure_result configure(ventura::absolute_path const &module_temporaries,
	                                ventura::absolute_path const &module_permanent,
	                                ventura::absolute_path const &application_source,
	                                ventura::absolute_path const &application_build_dir, unsigned cpu_parallelism,
	                                Si::Sink<char, Si::success>::interface &output)
	{
		boost::ignore_unused_variable_warning(cpu_parallelism);
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
		ventura::absolute_path const source = *cdm / "original_sources/Catch-1.2.1";
		cdm::catch_paths const installed = cdm::install_catch(source, module_temporaries, module_permanent, output);
		std::vector<Si::os_string> arguments;
		arguments.emplace_back(SILICIUM_OS_STR("-DCATCH_INCLUDE_DIRS=") + to_os_string(installed.include));
#ifdef _MSC_VER
		arguments.emplace_back(SILICIUM_OS_STR("-G \"Visual Studio 12 2013\""));
#endif
		arguments.emplace_back(to_os_string(application_source));
		if (ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output).get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}
		return cdm::configure_result();
	}
}
