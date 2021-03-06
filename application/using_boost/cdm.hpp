#include "cdm_boost.hpp"
#include <cdm/cmake_generator.hpp>
#include <ventura/cmake.hpp>
#include <ventura/run_process.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	void configure(ventura::absolute_path const &module_temporaries, ventura::absolute_path const &module_permanent,
	               ventura::absolute_path const &application_source,
	               ventura::absolute_path const &application_build_dir, unsigned cpu_parallelism,
	               cdm::operating_system const &system, cdm::configuration const &target,
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
		ventura::absolute_path const boost_source = *cdm / "original_sources/boost_1_59_0";
		cdm::boost_paths const boost_installed = cdm::install_boost(boost_source, module_temporaries, module_permanent,
		                                                            cpu_parallelism, system, target, output);
		std::vector<Si::noexcept_string> arguments;
		cdm::generate_cmake_definitions_for_using_boost(Si::make_container_sink(arguments), boost_installed.root);
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
