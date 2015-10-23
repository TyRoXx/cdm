#include "cdm_sqlite.hpp"
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
		ventura::absolute_path const original_source = *cdm / "original_sources/sqlite-autoconf-3081101";
		cdm::sqlite_paths const installed =
		    cdm::install_sqlite(original_source, module_temporaries, module_permanent, ventura::cmake_exe, output);
		std::vector<Si::os_string> arguments;
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DSQLITE3_INCLUDE_DIRS=") + to_os_string(installed.include));
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DSQLITE3_LIBRARIES=") + to_os_string(installed.library));
#ifdef _MSC_VER
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-G \"Visual Studio 12 2013\""));
#endif
		arguments.push_back(to_os_string(application_source));
		if (ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output).get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}
		return cdm::configure_result();
	}
}
