#include "cdm_libgit2.hpp"
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
		ventura::absolute_path const original_source = *cdm / ventura::relative_path("original_sources/libgit2-0.23.2");
		cdm::libgit2_paths const installed = cdm::install_libgit2(original_source, module_temporaries, module_permanent,
		                                                          ventura::cmake_exe, cpu_parallelism, output);
		std::vector<Si::os_string> arguments;
		arguments.push_back(Si::os_string(SILICIUM_SYSTEM_LITERAL("-DLIBGIT2_INCLUDE_DIR=")) +
		                    to_os_string(installed.include));
		arguments.push_back(Si::os_string(SILICIUM_SYSTEM_LITERAL("-DLIBGIT2_LIBRARY_DIR=")) +
		                    to_os_string(installed.library_dir));
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
