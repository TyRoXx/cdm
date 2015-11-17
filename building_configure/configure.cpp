#include "configure.hpp"
#include "cdm_boost.hpp"
#include <cdm/cmake_generator.hpp>
#include <ventura/file_operations.hpp>
#include <ventura/cmake.hpp>
#include <fstream>

namespace
{
	ventura::absolute_path build_configure(ventura::absolute_path const &application_source,
	                                       ventura::absolute_path const &temporary,
	                                       Si::optional<ventura::absolute_path> const &boost_root,
	                                       cdm::configuration const &target,
	                                       Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const repository =
		    ventura::parent(ventura::parent(*ventura::absolute_path::create(__FILE__))
		                        .or_throw([]
		                                  {
			                                  throw std::runtime_error(
			                                      "Could not find parent directory of this file: " __FILE__);
			                              }))
		        .or_throw([]
		                  {
			                  throw std::runtime_error("Could not find the repository directory");
			              });
		ventura::absolute_path const original_main_cpp = repository / "configure_cmdline/main.cpp";
		ventura::absolute_path const source = temporary / "source";
		ventura::recreate_directories(source, Si::throw_);
		ventura::absolute_path const copied_main_cpp = source / "main.cpp";
		ventura::copy(original_main_cpp, copied_main_cpp, Si::throw_);
		{
			ventura::absolute_path const cmakeLists = source / "CMakeLists.txt";
			std::ofstream cmakeListsFile(cmakeLists.c_str());
			cmakeListsFile << "cmake_minimum_required(VERSION 2.8)\n";
			cmakeListsFile << "project(configure_cmdline_generated)\n";
			cmakeListsFile << "if(UNIX)\n";
			cmakeListsFile << "  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion "
			                  "OUTPUT_VARIABLE GCC_VERSION)\n";
			cmakeListsFile << "  if(GCC_VERSION VERSION_GREATER 4.7)\n";
			cmakeListsFile << "    add_definitions(-std=c++1y)\n";
			cmakeListsFile << "  else()\n";
			cmakeListsFile << "    add_definitions(-std=c++0x)\n";
			cmakeListsFile << "  endif()\n";
			cmakeListsFile << "endif()\n";
			cmakeListsFile << "if(MSVC)\n";
			cmakeListsFile << "	set(Boost_USE_STATIC_LIBS ON)\n";
			cmakeListsFile << "	set(CMAKE_EXE_LINKER_FLAGS \"${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO\")\n";
			cmakeListsFile << "	add_definitions(-D_WIN32_WINDOWS)\n";
			cmakeListsFile << "	add_definitions(-DBOOST_ASIO_HAS_IOCP)\n";
			cmakeListsFile << "endif()\n";
			cmakeListsFile << "find_package(Boost REQUIRED filesystem coroutine date_time "
			                  "program_options thread chrono context regex system)\n";
			cmakeListsFile << "include_directories(SYSTEM ${SILICIUM_INCLUDE_DIR} "
			                  "${VENTURA_INCLUDE_DIR} ${Boost_INCLUDE_DIR} "
			                  "${CDM_CONFIGURE_INCLUDE_DIRS})\n";
			cmakeListsFile << "link_directories(${Boost_LIBRARY_DIR})\n";
			cmakeListsFile << "add_executable(configure main.cpp)\n";
			cmakeListsFile << "target_link_libraries(configure ${Boost_LIBRARIES})\n";
			if (!cmakeListsFile)
			{
				throw std::runtime_error(("Could not generate " + to_utf8_string(cmakeLists)).c_str());
			}
		}
		ventura::absolute_path const build = temporary / "build";
		ventura::recreate_directories(build, Si::throw_);
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
			if (boost_root)
			{
				cdm::generate_cmake_definitions_for_using_boost(Si::make_container_sink(arguments), *boost_root);
#if CDM_TESTS_RUNNING_ON_APPVEYOR
				arguments.emplace_back("-DBOOST_LIBRARYDIR=" +
				                       ventura::to_utf8_string(*boost_root / "lib32-msvc-14.0"));
#endif
			}
			ventura::absolute_path const modules = repository / "modules";
			arguments.emplace_back("-DCDM_CONFIGURE_INCLUDE_DIRS=" + ventura::to_utf8_string(application_source) + ";" +
			                       ventura::to_utf8_string(modules) + ";" + ventura::to_utf8_string(repository));
			arguments.emplace_back(ventura::to_utf8_string(source));
			cdm::generate_default_cmake_generator_arguments(Si::make_container_sink(arguments), target);
			if (ventura::run_process(ventura::cmake_exe, arguments, build, output,
			                         std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
			                         ventura::environment_inheritance::inherit).get() != 0)
			{
				throw std::runtime_error("Could not CMake-configure the cdm configure executable");
			}
		}
		{
			std::vector<Si::os_string> arguments;
			arguments.emplace_back(SILICIUM_OS_STR("--build"));
			arguments.emplace_back(SILICIUM_OS_STR("."));
			if (ventura::run_process(ventura::cmake_exe, arguments, build, output,
			                         std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
			                         ventura::environment_inheritance::inherit).get() != 0)
			{
				throw std::runtime_error("Could not CMake --build the cdm configure executable");
			}
		}
		ventura::absolute_path built_executable = build / ventura::relative_path(
#ifdef _MSC_VER
		                                                      "Debug/"
#endif
		                                                      "configure"
#ifdef _WIN32
		                                                      ".exe"
#endif
		                                                      );
		return built_executable;
	}

	void run_configure(ventura::absolute_path const &configure_executable,
	                   ventura::absolute_path const &module_permanent, ventura::absolute_path const &application_source,
	                   ventura::absolute_path const &application_build_dir,
	                   Si::Sink<char, Si::success>::interface &output)
	{
		ventura::create_directories(module_permanent, Si::throw_);
		ventura::create_directories(application_build_dir, Si::throw_);
		std::vector<Si::os_string> arguments;
		arguments.emplace_back(SILICIUM_OS_STR("-m"));
		arguments.emplace_back(to_os_string(module_permanent));
		arguments.emplace_back(SILICIUM_OS_STR("-a"));
		arguments.emplace_back(to_os_string(application_source));
		arguments.emplace_back(SILICIUM_OS_STR("-b"));
		arguments.emplace_back(to_os_string(application_build_dir));
		int const rc = ventura::run_process(configure_executable, arguments, application_build_dir, output,
		                                    std::vector<std::pair<Si::os_char const *, Si::os_char const *>>(),
		                                    ventura::environment_inheritance::inherit).get();
		if (rc != 0)
		{
			throw std::runtime_error("Could not configure the application: " + boost::lexical_cast<std::string>(rc));
		}
	}
}

namespace cdm
{
	void do_configure(ventura::absolute_path const &temporary, ventura::absolute_path const &module_permanent,
	                  ventura::absolute_path const &application_source,
	                  ventura::absolute_path const &application_build_dir,
	                  Si::optional<ventura::absolute_path> const &boost_root, cdm::configuration const &target,
	                  Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const configure_executable =
		    build_configure(application_source, temporary, boost_root, target, output);
		run_configure(configure_executable, module_permanent, application_source, application_build_dir, output);
	}
}
