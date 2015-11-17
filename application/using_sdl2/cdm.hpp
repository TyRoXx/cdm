#include "cdm_sdl2.hpp"
#include <ventura/cmake.hpp>
#include <ventura/file_operations.hpp>
#include <boost/thread/thread.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	void configure(ventura::absolute_path const &module_temporaries, ventura::absolute_path const &module_permanent,
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

		ventura::absolute_path const sdl2_source = *cdm / "original_sources/SDL2-2.0.3";
		cdm::sdl2_paths const sdl2_installed = cdm::install_sdl2(sdl2_source, module_temporaries, module_permanent,
		                                                         ventura::cmake_exe, cpu_parallelism, output);

		std::vector<Si::noexcept_string> arguments;
		arguments.emplace_back("-DSDL2_INCLUDE_DIR=" + ventura::to_utf8_string(sdl2_installed.include));
		arguments.emplace_back("-DSDL2_LIBRARY=" + ventura::to_utf8_string(sdl2_installed.library) + ";" +
		                       ventura::to_utf8_string(sdl2_installed.main));
		cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments));
		arguments.emplace_back(ventura::to_utf8_string(application_source));
		if (ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output,
		                         std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
		                         ventura::environment_inheritance::inherit)
		        .get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}

		ventura::copy(sdl2_installed.runtime_library,
		              application_build_dir / ventura::leaf(sdl2_installed.runtime_library), Si::throw_);
	}
}
