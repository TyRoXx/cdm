#include <cdm/cmake_argument_builder.hpp>
#include "cdm_gtest.hpp"
#include "cdm_cppnetlib.hpp"
#include <cdm/locate_cache.hpp>
#include <ventura/cmake.hpp>
#include <ventura/file_operations.hpp>
#include <boost/thread/thread.hpp>
#include <silicium/sink/function_sink.hpp>
#include <silicium/sink/ostream_sink.hpp>
#include <iostream>

namespace cdm
{
	template <class StringSink>
	void generate_cmake_application_arguments(StringSink &&arguments, ventura::absolute_path const &module_temporaries,
	                                          ventura::absolute_path const &module_permanent,
	                                          ventura::absolute_path const &application_source,
	                                          unsigned cpu_parallelism, cdm::configuration target,
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
		                           ventura::cmake_exe, cpu_parallelism, target, output);

		cmake_argument_builder<decltype(arguments) &> argument_builder(arguments);
		argument_builder.define("CPPNETLIB_PREFIX_PATH",
		                        ventura::to_utf8_string(cppnetlib_installed.cmake_prefix_path));
		argument_builder.define("GTEST_INCLUDE_DIRS", ventura::to_utf8_string(gtest_installed.include));
		argument_builder.define("GTEST_LIBRARIES", ventura::to_utf8_string(gtest_installed.library) + ";" +
		                                               ventura::to_utf8_string(gtest_installed.library_main));
		cdm::generate_default_cmake_generator_arguments(arguments);
		Si::append(arguments, ventura::to_utf8_string(application_source));
	}
}

int main()
{
	ventura::absolute_path const module_temporaries = ventura::temporary_directory(Si::throw_) / "cdm_modules";
	ventura::absolute_path const this_file = *ventura::absolute_path::create(__FILE__);
	ventura::absolute_path const application = *ventura::parent(*ventura::parent(this_file));
	unsigned const cpu_parallelism =
#if CDM_TESTS_RUNNING_ON_TRAVIS_CI
	    2;
#else
	    boost::thread::hardware_concurrency();
#endif
	auto output = Si::Sink<char, Si::success>::erase(Si::ostream_ref_sink(std::cerr));
	bool needs_space = false;
	cdm::generate_cmake_application_arguments(
	    Si::make_function_sink<Si::noexcept_string>(
	        [&needs_space](Si::iterator_range<Si::noexcept_string const *> arguments)
	        {
		        for (auto const &argument : arguments)
		        {
			        if (needs_space)
			        {
				        std::cout << ' ';
			        }
			        needs_space = true;
			        std::cout << argument;
		        }
		    }),
	    module_temporaries, cdm::locate_cache_for_this_binary(), application, cpu_parallelism,
	    cdm::approximate_configuration_of_this_binary(), output);
	std::cout << '\n';
}
