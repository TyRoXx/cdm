#include "configure.hpp"
#include <ventura/file_operations.hpp>
#include <ventura/cmake.hpp>
#include <fstream>

namespace
{
	ventura::absolute_path build_configure(ventura::absolute_path const &application_source,
	                                       ventura::absolute_path const &temporary,
	                                       Si::optional<ventura::absolute_path> const &boost_root,
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
			std::vector<Si::os_string> arguments;
			{
				ventura::absolute_path const include = repository / "dependencies/silicium";
				arguments.emplace_back(SILICIUM_OS_STR("-DSILICIUM_INCLUDE_DIR=") + to_os_string(include));
			}
			{
				ventura::absolute_path const include = repository / "dependencies/ventura";
				arguments.emplace_back(SILICIUM_OS_STR("-DVENTURA_INCLUDE_DIR=") + to_os_string(include));
			}
			if (boost_root)
			{
				arguments.emplace_back(SILICIUM_OS_STR("-DBOOST_ROOT=") + to_os_string(*boost_root));
				arguments.emplace_back(SILICIUM_OS_STR("-DBoost_NO_SYSTEM_PATHS=ON"));
			}
			ventura::absolute_path const modules = repository / "modules";
			arguments.emplace_back(SILICIUM_OS_STR("-DCDM_CONFIGURE_INCLUDE_DIRS=") +
			                       to_os_string(application_source) + SILICIUM_OS_STR(";") +
			                       to_os_string(modules) + SILICIUM_OS_STR(";") + to_os_string(repository));
			arguments.emplace_back(to_os_string(source));
#ifdef _MSC_VER
			arguments.emplace_back(SILICIUM_OS_STR("-G \"Visual Studio 12 2013\""));
#endif
			if (ventura::run_process(ventura::cmake_exe, arguments, build, output).get() != 0)
			{
				throw std::runtime_error("Could not CMake-configure the cdm configure executable");
			}
		}
		{
			std::vector<Si::os_string> arguments;
			arguments.emplace_back(SILICIUM_OS_STR("--build"));
			arguments.emplace_back(SILICIUM_OS_STR("."));
			if (ventura::run_process(ventura::cmake_exe, arguments, build, output).get() != 0)
			{
				throw std::runtime_error("Could not CMake --build the cdm configure executable");
			}
		}
		ventura::absolute_path built_executable = build / ventura::relative_path(
#ifdef _MSC_VER
		                                                      SILICIUM_OS_STR("Debug/")
#endif
		                                                          SILICIUM_OS_STR("configure")
#ifdef _WIN32
		                                                              SILICIUM_OS_STR(".exe")
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
		int const rc = ventura::run_process(configure_executable, arguments, application_build_dir, output).get();
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
	                  Si::optional<ventura::absolute_path> const &boost_root,
	                  Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const configure_executable =
		    build_configure(application_source, temporary, boost_root, output);
		run_configure(configure_executable, module_permanent, application_source, application_build_dir, output);
	}
}
