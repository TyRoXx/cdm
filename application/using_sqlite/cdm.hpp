#include "cdm_sqlite.hpp"
#include <cdm/cmake_generator.hpp>
#include <cdm/operating_system.hpp>
#include <ventura/cmake.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	void configure(ventura::absolute_path const &module_temporaries, ventura::absolute_path const &module_permanent,
	               ventura::absolute_path const &application_source,
	               ventura::absolute_path const &application_build_dir, unsigned cpu_parallelism,
	               cdm::operating_system const &, cdm::configuration const &target,
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
		cdm::sqlite_paths const installed = cdm::install_sqlite(original_source, module_temporaries, module_permanent,
		                                                        ventura::cmake_exe, target, output);
		std::vector<Si::noexcept_string> arguments;
		arguments.emplace_back("-DSQLITE3_INCLUDE_DIRS=" + ventura::to_utf8_string(installed.include));
		arguments.emplace_back("-DSQLITE3_LIBRARIES=" + ventura::to_utf8_string(installed.library));
		cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments), target);
		arguments.emplace_back(ventura::to_utf8_string(application_source));
		if (ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output,
		                         std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
		                         ventura::environment_inheritance::inherit)
		        .get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}
	}
}
