#include "cdm_gtest.hpp"
#include "cdm_cppnetlib.hpp"
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

		ventura::absolute_path const gtest_source = *cdm / "original_sources/gtest-1.7.0";
		cdm::gtest_paths const gtest_installed =
		    cdm::install_gtest(gtest_source, module_temporaries, module_permanent, ventura::cmake_exe, output);

		ventura::absolute_path const cppnetlib_source = *cdm / "original_sources/cpp-netlib-0.11.2-final";
		ventura::recreate_directories(module_temporaries, Si::throw_);
		ventura::absolute_path const boost_source = *cdm / "original_sources/boost_1_59_0";
		cdm::cppnetlib_paths const cppnetlib_installed =
		    cdm::install_cppnetlib(cppnetlib_source, boost_source, module_temporaries, module_permanent,
		                           ventura::cmake_exe, cpu_parallelism, output);

		std::vector<Si::os_string> arguments;
		arguments.emplace_back(SILICIUM_OS_STR("-DCPPNETLIB_PREFIX_PATH=") +
		                       to_os_string(cppnetlib_installed.cmake_prefix_path));
		arguments.emplace_back(SILICIUM_OS_STR("-DGTEST_INCLUDE_DIRS=") + to_os_string(gtest_installed.include));
		arguments.emplace_back(SILICIUM_OS_STR("-DGTEST_LIBRARIES=") + to_os_string(gtest_installed.library) +
		                       SILICIUM_OS_STR(";") + to_os_string(gtest_installed.library_main));
		cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments));
		arguments.emplace_back(to_os_string(application_source));
		if (ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output).get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}
	}
}
