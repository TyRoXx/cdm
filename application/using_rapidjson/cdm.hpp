#include "cdm_rapidjson.hpp"
#include <silicium/cmake.hpp>
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
		boost::ignore_unused_variable_warning(cpu_parallelism);
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
		Si::absolute_path const source = *cdm / Si::relative_path("original_sources/rapidjson-1.0.2");
		cdm::rapidjson_paths const installed = cdm::install_rapidjson(source, module_temporaries, module_permanent, output);
		std::vector<Si::os_string> arguments;
		arguments.push_back(Si::os_string(SILICIUM_SYSTEM_LITERAL("-DRAPIDJSON_INCLUDE_DIR=")) + to_os_string(installed.include));
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
