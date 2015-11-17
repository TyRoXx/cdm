#include "cdm_libgit2.hpp"
#include <cdm/cmake_generator.hpp>
#include <cdm/operating_system.hpp>
#include <ventura/cmake.hpp>
#include <silicium/sink/iterator_sink.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	void configure(ventura::absolute_path const &module_temporaries, ventura::absolute_path const &module_permanent,
	               ventura::absolute_path const &application_source,
	               ventura::absolute_path const &application_build_dir, unsigned cpu_parallelism,
	               cdm::operating_system const &, cdm::configuration const &target,
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
		ventura::absolute_path const original_source = *cdm / "original_sources/libgit2-0.23.2";
		cdm::libgit2_paths const installed = cdm::install_libgit2(original_source, module_temporaries, module_permanent,
		                                                          ventura::cmake_exe, cpu_parallelism, target, output);
		std::vector<Si::noexcept_string> arguments;
		arguments.emplace_back("-DLIBGIT2_INCLUDE_DIR=" + ventura::to_utf8_string(installed.include));
		arguments.emplace_back("-DLIBGIT2_LIBRARY_DIR=" + ventura::to_utf8_string(installed.library_dir));
		cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments), target);
		arguments.emplace_back(ventura::to_utf8_string(application_source));
		if (ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output,
		                         std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
		                         ventura::environment_inheritance::inherit).get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}
	}
}
