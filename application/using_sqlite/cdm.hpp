#include "cdm_sqlite.hpp"
#include <silicium/cmake.hpp>
#include <cdm/configure_result.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	cdm::configure_result configure(
		Si::absolute_path const &module_temporaries,
		Si::absolute_path const &module_permanent,
		Si::absolute_path const &application_source,
		Si::absolute_path const &application_build_dir,
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
		Si::absolute_path const original_source = *cdm / Si::relative_path("original_sources/sqlite-autoconf-3081101");
		cdm::sqlite_paths const installed = cdm::install_sqlite(original_source, module_temporaries, module_permanent, Si::cmake_exe, output);
		std::vector<Si::os_string> arguments;
		arguments.push_back(Si::os_string(SILICIUM_SYSTEM_LITERAL("-DSQLITE3_INCLUDE_DIRS=")) + to_os_string(installed.include));
		arguments.push_back(Si::os_string(SILICIUM_SYSTEM_LITERAL("-DSQLITE3_LIBRARIES=")) + to_os_string(installed.library));
#ifdef _MSC_VER
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-G \"Visual Studio 12 2013\""));
#endif
		arguments.push_back(Si::to_os_string(application_source));
		if (Si::run_process(Si::cmake_exe, arguments, application_build_dir, output).get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}
		return cdm::configure_result();
	}
}
