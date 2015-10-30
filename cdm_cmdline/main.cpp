#include "building_configure/configure.hpp"
#include <silicium/program_options.hpp>
#include <ventura/file_operations.hpp>
#include <silicium/sink/ostream_sink.hpp>
#include <iostream>

#ifdef _MSC_VER
#define SILICIUM_WHILE_FALSE while (0, 0)
#else
#define SILICIUM_WHILE_FALSE while (false)
#endif

#define LOG(...)                                                                                                       \
	do                                                                                                                 \
	{                                                                                                                  \
		std::cerr << __VA_ARGS__ << '\n';                                                                              \
	}                                                                                                                  \
	SILICIUM_WHILE_FALSE

int main(int argc, char **argv)
{
	std::string module_permanent_argument = to_utf8_string<std::string>(ventura::get_home() / ".cdm_cache");
	std::string application_source_argument;
	std::string application_build_argument;
	std::string temporary_root_argument = to_utf8_string<std::string>(ventura::temporary_directory(Si::throw_) / "cdm_cmdline");

	boost::program_options::options_description options("options");
	options.add_options()("help,h", "produce help message")(
	    "modules,m", boost::program_options::value(&module_permanent_argument),
	    ("path to the permanent module binary cache (default " + module_permanent_argument + ")").c_str())(
	    "application,a", boost::program_options::value(&application_source_argument),
	    "path to the root of your application source code")("build,b",
	                                                        boost::program_options::value(&application_build_argument),
	                                                        "path to the CMake build directory of your application")(
	    "temporary,t", boost::program_options::value(&temporary_root_argument),
	    ("temporary directory for the generated CMake project (default " + temporary_root_argument + ")").c_str());
	boost::program_options::positional_options_description positional;
	boost::program_options::variables_map variables;
	try
	{
		boost::program_options::store(
		    boost::program_options::command_line_parser(argc, argv).options(options).positional(positional).run(),
		    variables);
		boost::program_options::notify(variables);
	}
	catch (boost::program_options::error const &ex)
	{
		LOG(options);
		LOG(ex.what());
		return 1;
	}

	if (variables.count("help") || module_permanent_argument.empty() || application_source_argument.empty() ||
	    application_build_argument.empty())
	{
		LOG(options);
		return 0;
	}

	try
	{
		ventura::absolute_path const module_permanent =
		    ventura::absolute_path::create(boost::filesystem::absolute(module_permanent_argument))
		        .or_throw(
		            []
		            {
			            throw std::invalid_argument("The permanent module cache argument must be an absolute path.");
			        });
		ventura::absolute_path const application_source =
		    ventura::absolute_path::create(boost::filesystem::absolute(application_source_argument))
		        .or_throw([]
		                  {
			                  throw std::invalid_argument("The application source argument must be an absolute path.");
			              });
		ventura::absolute_path const application_build =
		    ventura::absolute_path::create(boost::filesystem::absolute(application_build_argument))
		        .or_throw([]
		                  {
			                  throw std::invalid_argument(
			                      "The application build directory argument must be an absolute path.");
			              });
		ventura::absolute_path const temporary_root =
		    ventura::absolute_path::create(boost::filesystem::absolute(temporary_root_argument))
		        .or_throw([]
		                  {
			                  throw std::invalid_argument("The temporary directory argument must be an absolute path.");
			              });
		auto output = Si::Sink<char, Si::success>::erase(Si::ostream_ref_sink(std::cerr));
		cdm::do_configure(temporary_root, module_permanent, application_source, application_build, Si::none, output);
		LOG("Your application has been configured.");
	}
	catch (std::exception const &ex)
	{
		LOG(ex.what());
		return 1;
	}
}
