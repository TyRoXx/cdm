#include "cdm_boost.hpp"
#include "cdm_catch.hpp"
#include "cdm_cppnetlib.hpp"
#include "cdm_libgit2.hpp"
#include "cdm_rapidjson.hpp"
#include "cdm_rapidxml.hpp"
#include "cdm_sqlite.hpp"
#include "cdm_websocketpp.hpp"
#include <silicium/cmake.hpp>
#include <silicium/run_process.hpp>
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

		Si::absolute_path const boost_source = *cdm / Si::relative_path("original_sources/boost_1_59_0");
		cdm::boost_paths const boost_installed = cdm::install_boost(boost_source, module_temporaries, module_permanent, cpu_parallelism, output);

		Si::absolute_path const catch_source = *cdm / Si::relative_path("original_sources/Catch-1.2.1");
		cdm::catch_paths const catch_installed = cdm::install_catch(catch_source, module_temporaries, module_permanent, output);

		Si::absolute_path const cppnetlib_source = *cdm / Si::relative_path("original_sources/cpp-netlib-0.11.2-final");
		cdm::cppnetlib_paths const cppnetlib_installed = cdm::install_cppnetlib(cppnetlib_source, boost_source, module_temporaries, module_permanent, Si::cmake_exe, cpu_parallelism, output);

		Si::absolute_path const libgit2_source = *cdm / Si::relative_path("original_sources/libgit2-0.23.2");
		cdm::libgit2_paths const libgit2installed = cdm::install_libgit2(libgit2_source, module_temporaries, module_permanent, Si::cmake_exe, cpu_parallelism, output);

		Si::absolute_path const rapidjson_source = *cdm / Si::relative_path("original_sources/rapidjson-1.0.2");
		cdm::rapidjson_paths const rapidjson_installed = cdm::install_rapidjson(rapidjson_source, module_temporaries, module_permanent, output);

		Si::absolute_path const rapidxml_source = *cdm / Si::relative_path("original_sources/rapidxml-1.13");
		cdm::rapidxml_paths const rapidxml_installed = cdm::install_rapidxml(rapidxml_source, module_temporaries, module_permanent, output);

		Si::absolute_path const sqlite3_source = *cdm / Si::relative_path("original_sources/sqlite-autoconf-3081101");
		cdm::sqlite_paths const sqlite3_installed = cdm::install_sqlite(sqlite3_source, module_temporaries, module_permanent, Si::cmake_exe, output);

		Si::absolute_path const websocketpp_source = *cdm / Si::relative_path("original_sources/websocketpp-c5510d6de04917812b910a8dd44735c1f17061d9");
		cdm::websocketpp_paths const websocketpp_installed = cdm::install_websocketpp(websocketpp_source, boost_source, module_temporaries, module_permanent, cpu_parallelism, output);

		std::vector<Si::os_string> arguments;
#if 0
		Si::os_string const our_boost_root = to_os_string(boost_installed.root);
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBOOST_ROOT=") + our_boost_root);
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBoost_ADDITIONAL_VERSIONS=1.59"));
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBoost_NO_SYSTEM_PATHS=ON"));
#endif
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DCATCH_INCLUDE_DIRS=") + to_os_string(catch_installed.include));
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DCPPNETLIB_PREFIX_PATH=") + to_os_string(cppnetlib_installed.cmake_prefix_path));
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DRAPIDJSON_INCLUDE_DIR=") + to_os_string(rapidjson_installed.include));
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DRAPIDXML_INCLUDE_DIR=") + to_os_string(rapidxml_installed.include));
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DLIBGIT2_INCLUDE_DIR=") + to_os_string(libgit2installed.include));
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DLIBGIT2_LIBRARY_DIR=") + to_os_string(libgit2installed.library_dir));
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DSQLITE3_INCLUDE_DIRS=") + to_os_string(sqlite3_installed.include));
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DSQLITE3_LIBRARIES=") + to_os_string(sqlite3_installed.library));
		arguments.push_back(SILICIUM_SYSTEM_LITERAL("-DWEBSOCKETPP_INCLUDE_DIR=") + to_os_string(websocketpp_installed.include));
#ifdef _MSC_VER
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-G \"Visual Studio 12 2013\""));
#endif
		arguments.push_back(Si::to_os_string(application_source));
		if (Si::run_process(Si::cmake_exe, arguments, application_build_dir, output).get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}

		cdm::configure_result result;
		result.shared_library_directories.emplace_back(boost_installed.root / Si::relative_path("lib"));
		return result;
	}
}