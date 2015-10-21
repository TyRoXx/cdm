#ifndef CDM_BOOST_HPP
#define CDM_BOOST_HPP

#include <ventura/file_operations.hpp>
#include <ventura/run_process.hpp>
#include <silicium/sink/iterator_sink.hpp>
#include <boost/lexical_cast.hpp>

// Boost is so large that the output of cp exceeds the 4 MB log limit of travis
// which aborts the build job then.
// On Windows, the console is so slow (especially in Virtualbox) that we want
// to avoid the megabytes of output from the Boost build process.
// TODO: Buffer the output in memory and save it somewhere if something fails.
#if CDM_TESTS_RUNNING_ON_TRAVIS_CI || defined(_WIN32)
#define CDM_AVOID_CONSOLE_OUTPUT 1
#else
#define CDM_AVOID_CONSOLE_OUTPUT 0
#endif

namespace cdm
{
	struct boost_paths
	{
		ventura::absolute_path root;
	};

	inline boost_paths install_boost(ventura::absolute_path const &source, ventura::absolute_path const &temporary,
	                                 ventura::absolute_path const &install_root, unsigned make_parallelism,
	                                 Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const module_in_cache = install_root / ventura::relative_path("boost");
		if (!ventura::file_exists(module_in_cache, Si::throw_))
		{
			ventura::absolute_path const copy_of_boost = temporary / ventura::relative_path("src");
			ventura::copy_recursively(source, copy_of_boost,
#if CDM_AVOID_CONSOLE_OUTPUT
			                          nullptr
#else
			                          &output
#endif
			                          ,
			                          Si::throw_);

			ventura::create_directories(module_in_cache, Si::throw_);

			{
				std::vector<Si::os_string> arguments;
#ifdef _MSC_VER
				ventura::absolute_path const exe = copy_of_boost / "bootstrap.bat";
#else
				ventura::absolute_path const exe = *ventura::absolute_path::create("/usr/bin/env");
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("bash"));
				arguments.push_back(to_os_string(copy_of_boost / "bootstrap.sh"));
#endif
				int const rc = ventura::run_process(exe, arguments, copy_of_boost, output).get();
				if (rc != 0)
				{
					throw std::runtime_error("bootstrap failed");
				}
			}

			{
#if CDM_AVOID_CONSOLE_OUTPUT
				std::vector<char> output_buffer;
				auto buffering = Si::Sink<char, Si::success>::erase(Si::make_container_sink(output_buffer));
#endif
				std::vector<Si::os_string> arguments;
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("install"));
				{
					Si::os_string const install_argument =
					    SILICIUM_SYSTEM_LITERAL("--prefix=") + to_os_string(module_in_cache);
					arguments.push_back(install_argument);
				}
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("link=static"));
#ifdef _MSC_VER
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("toolset=msvc-12.0"));
#endif
#ifndef CDM_TESTS_RUNNING_ON_TRAVIS_CI
				// GCC 4.6 crashes when compiling Boost.Log on travis probably due
				// to lack of RAM.
				// Thus we do not parallelize the build on travis so that the
				// compiler can use all of the memory available to the machine.
				arguments.push_back(SILICIUM_SYSTEM_LITERAL("-j ") +
				                    boost::lexical_cast<Si::os_string>(make_parallelism));
#else
				boost::ignore_unused_variable_warning(make_parallelism);
#endif
				int const rc = ventura::run_process(copy_of_boost / "b2"
#ifdef _WIN32
				                                                    ".exe"
#endif
				                                    ,
				                                    arguments, copy_of_boost,
#if CDM_AVOID_CONSOLE_OUTPUT
				                                    buffering
#else
				                                    output
#endif
				                                    ).get();
				if (rc != 0)
				{
#if CDM_AVOID_CONSOLE_OUTPUT
					// TODO: do not buffer more characters than necessary
					std::size_t const max_output = 3500 * 1000;
					std::size_t const actual_output = (std::min)(max_output, output_buffer.size());
					char const *const end = output_buffer.data() + output_buffer.size();
					Si::append_range(output, Si::make_iterator_range(end - actual_output, end));
#endif
					throw std::runtime_error("b2 failed");
				}
			}
		}
		boost_paths result;
		result.root = module_in_cache;
		return result;
	}
}

#endif
