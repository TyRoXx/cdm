#include "cdm_rapidxml.hpp"
#include <ventura/cmake.hpp>
#include <silicium/sink/iterator_sink.hpp>
#include <cdm/cmake_generator.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	void configure(ventura::absolute_path const &module_temporaries, ventura::absolute_path const &module_permanent,
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
		ventura::absolute_path const source = *cdm / "original_sources/rapidxml-1.13";
		cdm::rapidxml_paths const installed =
		    cdm::install_rapidxml(source, module_temporaries, module_permanent, output);
		std::vector<Si::noexcept_string> arguments;
		arguments.emplace_back("-DRAPIDXML_INCLUDE_DIR=" + ventura::to_utf8_string(installed.include));
		cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments));
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
