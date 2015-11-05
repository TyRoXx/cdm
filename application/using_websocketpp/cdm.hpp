#include "cdm_websocketpp.hpp"
#include <ventura/cmake.hpp>

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
		ventura::absolute_path const original_source =
		    *cdm / "original_sources/websocketpp-c5510d6de04917812b910a8dd44735c1f17061d9";
		ventura::absolute_path const boost_source = *cdm / "original_sources/boost_1_59_0";
		cdm::websocketpp_paths const installed = cdm::install_websocketpp(
		    original_source, boost_source, module_temporaries, module_permanent, cpu_parallelism, output);
		std::vector<Si::os_string> arguments;
		arguments.emplace_back(SILICIUM_OS_STR("-DWEBSOCKETPP_INCLUDE_DIR=") + to_os_string(installed.include));
		arguments.emplace_back(SILICIUM_OS_STR("-DBOOST_ROOT=") + to_os_string(installed.boost_root));
#if CDM_TESTS_RUNNING_ON_APPVEYOR
		arguments.emplace_back(SILICIUM_OS_STR("-DBOOST_LIBRARYDIR=") +
		                       to_os_string(installed.boost_root / "lib32-msvc-14.0"));
#endif
		arguments.emplace_back(SILICIUM_OS_STR("-DBoost_ADDITIONAL_VERSIONS=1.59"));
		arguments.emplace_back(SILICIUM_OS_STR("-DBoost_NO_SYSTEM_PATHS=ON"));
#ifdef _MSC_VER
		arguments.emplace_back(SILICIUM_OS_STR("-G \"Visual Studio 12 2013\""));
#endif
		arguments.emplace_back(to_os_string(application_source));
		if (ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output).get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}
	}
}
