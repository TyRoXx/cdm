#include "cdm_boost.hpp"
#include "cdm_catch.hpp"
#include "cdm_cppnetlib.hpp"
#include "cdm_libgit2.hpp"
#include "cdm_rapidjson.hpp"
#include "cdm_rapidxml.hpp"
#include "cdm_sqlite.hpp"
#include "cdm_websocketpp.hpp"
#include <ventura/cmake.hpp>
#include <ventura/run_process.hpp>

namespace CDM_CONFIGURE_NAMESPACE
{
	void configure(ventura::absolute_path const &module_temporaries, ventura::absolute_path const &module_permanent,
	               ventura::absolute_path const &application_source,
	               ventura::absolute_path const &application_build_dir, unsigned cpu_parallelism,
	               cdm::configuration target, Si::Sink<char, Si::success>::interface &output)
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
		ventura::absolute_path const sources = *cdm / "original_sources";

		ventura::absolute_path const boost_source = sources / "boost_1_59_0";
		cdm::boost_paths const boost_installed =
		    cdm::install_boost(boost_source, module_temporaries, module_permanent, cpu_parallelism, target, output);

		ventura::absolute_path const catch_source = sources / "Catch-1.2.1";
		cdm::catch_paths const catch_installed =
		    cdm::install_catch(catch_source, module_temporaries, module_permanent, output);

		ventura::absolute_path const cppnetlib_source = sources / "cpp-netlib-0.11.2-final";
		cdm::cppnetlib_paths const cppnetlib_installed =
		    cdm::install_cppnetlib(cppnetlib_source, boost_source, module_temporaries, module_permanent,
		                           ventura::cmake_exe, cpu_parallelism, target, output);

		ventura::absolute_path const libgit2_source = sources / "libgit2-0.23.2";
		cdm::libgit2_paths const libgit2installed = cdm::install_libgit2(
		    libgit2_source, module_temporaries, module_permanent, ventura::cmake_exe, cpu_parallelism, output);

		ventura::absolute_path const rapidjson_source = sources / "rapidjson-1.0.2";
		cdm::rapidjson_paths const rapidjson_installed =
		    cdm::install_rapidjson(rapidjson_source, module_temporaries, module_permanent, output);

		ventura::absolute_path const rapidxml_source = sources / "rapidxml-1.13";
		cdm::rapidxml_paths const rapidxml_installed =
		    cdm::install_rapidxml(rapidxml_source, module_temporaries, module_permanent, output);

		ventura::absolute_path const sqlite3_source = sources / "sqlite-autoconf-3081101";
		cdm::sqlite_paths const sqlite3_installed =
		    cdm::install_sqlite(sqlite3_source, module_temporaries, module_permanent, ventura::cmake_exe, output);

		ventura::absolute_path const websocketpp_source =
		    sources / "websocketpp-c5510d6de04917812b910a8dd44735c1f17061d9";
		cdm::websocketpp_paths const websocketpp_installed = cdm::install_websocketpp(
		    websocketpp_source, boost_source, module_temporaries, module_permanent, cpu_parallelism, target, output);

		std::vector<Si::noexcept_string> arguments;
		cdm::generate_cmake_definitions_for_using_boost(Si::make_container_sink(arguments), boost_installed.root);
		arguments.emplace_back("-DCATCH_INCLUDE_DIRS=" + ventura::to_utf8_string(catch_installed.include));
		arguments.emplace_back("-DCPPNETLIB_PREFIX_PATH=" +
		                       ventura::to_utf8_string(cppnetlib_installed.cmake_prefix_path));
		arguments.emplace_back("-DRAPIDJSON_INCLUDE_DIR=" + ventura::to_utf8_string(rapidjson_installed.include));
		arguments.emplace_back("-DRAPIDXML_INCLUDE_DIR=" + ventura::to_utf8_string(rapidxml_installed.include));
		arguments.emplace_back("-DLIBGIT2_INCLUDE_DIR=" + ventura::to_utf8_string(libgit2installed.include));
		arguments.emplace_back("-DLIBGIT2_LIBRARY_DIR=" + ventura::to_utf8_string(libgit2installed.library_dir));
		arguments.emplace_back("-DSQLITE3_INCLUDE_DIRS=" + ventura::to_utf8_string(sqlite3_installed.include));
		arguments.emplace_back("-DSQLITE3_LIBRARIES=" + ventura::to_utf8_string(sqlite3_installed.library));
		arguments.emplace_back("-DWEBSOCKETPP_INCLUDE_DIR=" + ventura::to_utf8_string(websocketpp_installed.include));
		cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments));
		arguments.emplace_back(ventura::to_utf8_string(application_source));
		if (ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output,
		                         std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
		                         ventura::environment_inheritance::inherit).get() != 0)
		{
			throw std::runtime_error("CMake configure failed");
		}
	}
}
