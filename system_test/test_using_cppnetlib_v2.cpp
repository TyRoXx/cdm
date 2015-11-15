#include "log.hpp"
#include "boost_root.hpp"
#include "temporary.hpp"
#include "cdm_boost.hpp"
#include "cdm/cmake_generator.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <silicium/sink/ostream_sink.hpp>
#include <cdm/locate_cache.hpp>
#include <ventura/cmake.hpp>
#include <boost/algorithm/string/split.hpp>

namespace
{
	ventura::absolute_path const this_file = *ventura::absolute_path::create(__FILE__);
	ventura::absolute_path const test = *ventura::parent(this_file);
	ventura::absolute_path const repository = *ventura::parent(test);
}

BOOST_AUTO_TEST_CASE(test_using_cppnetlib_v2)
{
	cdm::travis_keep_alive_printer keep_travis_alive;
	ventura::absolute_path const app_source = repository / "application/using_cppnetlib_v2";
	ventura::absolute_path const app_cdm_source = app_source / "cdm";
	ventura::absolute_path const tmp = cdm::get_temporary_root_for_testing() / "cdm";
	ventura::absolute_path const module_temporaries = tmp / "mod";
	ventura::absolute_path const application_build_dir = tmp / "using_cppnetlib_v2";
	ventura::recreate_directories(module_temporaries, Si::throw_);
	ventura::recreate_directories(application_build_dir, Si::throw_);
	std::unique_ptr<std::ofstream> log_file = cdm::open_log(application_build_dir / "test_using_cppnetlib_v2.txt");
	auto output = cdm::make_program_output_printer(Si::ostream_ref_sink(*log_file));
	ventura::absolute_path const build_configure = module_temporaries / "build_configure";
	ventura::recreate_directories(build_configure, Si::throw_);
	{
		std::vector<Si::noexcept_string> arguments;
		{
			ventura::absolute_path const include = repository / "dependencies/silicium";
			arguments.emplace_back("-DSILICIUM_INCLUDE_DIR=" + ventura::to_utf8_string(include));
		}
		{
			ventura::absolute_path const include = repository / "dependencies/ventura";
			arguments.emplace_back("-DVENTURA_INCLUDE_DIR=" + ventura::to_utf8_string(include));
		}
		Si::optional<ventura::absolute_path> const boost_root = cdm::get_boost_root_for_testing();
		if (boost_root)
		{
			cdm::generate_cmake_definitions_for_using_boost(Si::make_container_sink(arguments), *boost_root);
#if CDM_TESTS_RUNNING_ON_APPVEYOR
			arguments.emplace_back("-DBOOST_LIBRARYDIR=" + ventura::to_utf8_string(*boost_root / "lib32-msvc-14.0"));
#endif
		}
		ventura::absolute_path const modules = repository / "modules";
		arguments.emplace_back("-DCDM_CONFIGURE_INCLUDE_DIRS=" + ventura::to_utf8_string(modules) + ";" +
		                       ventura::to_utf8_string(repository));
		arguments.emplace_back(ventura::to_utf8_string(app_cdm_source));
		cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments));
		BOOST_REQUIRE_EQUAL(0, ventura::run_process(ventura::cmake_exe, arguments, build_configure, output,
		                                            std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
		                                            ventura::environment_inheritance::inherit).get());
	}

	{
		std::vector<Si::noexcept_string> arguments;
		arguments.emplace_back("--build");
		arguments.emplace_back(".");
		BOOST_REQUIRE_EQUAL(0, ventura::run_process(ventura::cmake_exe, arguments, build_configure, output,
		                                            std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
		                                            ventura::environment_inheritance::inherit).get());
	}

	{
		Si::noexcept_string generated_cmake_arguments;
		{
			std::vector<Si::noexcept_string> arguments;
			ventura::relative_path const relative(
#ifdef _WIN32
			    "Debug/"
#endif
			    "configure"
#ifdef _WIN32
			    ".exe"
#endif
			    );
			auto configure_sink =
			    Si::Sink<char, Si::success>::erase(Si::make_container_sink(generated_cmake_arguments));
			ventura::process_parameters parameters;
			parameters.executable = build_configure / relative;
			parameters.arguments = ventura::arguments_to_os_strings(arguments);
			parameters.current_path = build_configure;
			parameters.err = &output;
			parameters.out = &configure_sink;
			BOOST_REQUIRE_EQUAL(0, ventura::run_process(parameters));
			BOOST_CHECK(!generated_cmake_arguments.empty());
		}

		{
			std::vector<Si::noexcept_string> arguments;
			boost::algorithm::split(arguments, generated_cmake_arguments, [](char c)
			                        {
				                        return c >= 0 && c <= ' ';
				                    });
			BOOST_REQUIRE(!arguments.empty());
			BOOST_REQUIRE(arguments.back().empty());
			arguments.pop_back();
			cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments));
			BOOST_REQUIRE_EQUAL(0,
			                    ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output,
			                                         std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
			                                         ventura::environment_inheritance::inherit).get());
		}
	}

	{
		std::vector<Si::noexcept_string> arguments;
		arguments.emplace_back("--build");
		arguments.emplace_back(".");
		BOOST_REQUIRE_EQUAL(0, ventura::run_process(ventura::cmake_exe, arguments, application_build_dir, output,
		                                            std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
		                                            ventura::environment_inheritance::inherit).get());
	}

	{
		std::vector<Si::os_string> arguments;
		ventura::relative_path const relative(
#ifdef _WIN32
		    "Debug/"
#endif
		    "using_cppnetlib_v2"
#ifdef _WIN32
		    ".exe"
#endif
		    );
		BOOST_REQUIRE_EQUAL(0, ventura::run_process(application_build_dir / relative, arguments, application_build_dir,
		                                            output,
		                                            std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
		                                            ventura::environment_inheritance::inherit));
	}
}
