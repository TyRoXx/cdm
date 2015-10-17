#include "configure.hpp"
#include <silicium/file_operations.hpp>
#include <silicium/cmake.hpp>
#include <fstream>

namespace
{
	Si::absolute_path build_configure(
		Si::absolute_path const &application_source,
		Si::absolute_path const &temporary,
		Si::optional<Si::absolute_path> const &boost_root,
		Si::Sink<char, Si::success>::interface &output)
	{
		Si::absolute_path const repository = Si::parent(
			Si::parent(*Si::absolute_path::create(__FILE__)).or_throw(
				[]{ throw std::runtime_error("Could not find parent directory of this file: " __FILE__); }
			)
		).or_throw(
			[]{ throw std::runtime_error("Could not find the repository directory"); }
		);
		Si::absolute_path const original_main_cpp = repository / Si::relative_path("configure_cmdline/main.cpp");
		Si::absolute_path const source = temporary / Si::relative_path("source");
		Si::recreate_directories(source, Si::throw_);
		Si::absolute_path const copied_main_cpp = source / Si::relative_path("main.cpp");
		Si::copy(original_main_cpp, copied_main_cpp, Si::throw_);
		{
			Si::absolute_path const cmakeLists = source / Si::relative_path("CMakeLists.txt");
			std::ofstream cmakeListsFile(cmakeLists.c_str());
			cmakeListsFile << "cmake_minimum_required(VERSION 2.8)\n";
			cmakeListsFile << "project(configure_cmdline_generated)\n";
			cmakeListsFile << "if(UNIX)\n";
			cmakeListsFile << "  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)\n";
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
			cmakeListsFile << "find_package(Boost REQUIRED filesystem coroutine date_time program_options thread chrono context regex system)\n";
			cmakeListsFile << "include_directories(SYSTEM ${SILICIUM_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${CDM_CONFIGURE_INCLUDE_DIRS})\n";
			cmakeListsFile << "link_directories(${Boost_LIBRARY_DIR})\n";
			cmakeListsFile << "add_executable(configure main.cpp)\n";
			cmakeListsFile << "target_link_libraries(configure ${Boost_LIBRARIES})\n";
			if (!cmakeListsFile)
			{
				throw std::runtime_error(("Could not generate " + Si::to_utf8_string(cmakeLists)).c_str());
			}
		}
		Si::absolute_path const build = temporary / Si::relative_path("build");
		Si::recreate_directories(build, Si::throw_);
		{
			std::vector<Si::os_string> arguments;
			{
				Si::absolute_path const silicium = repository / Si::relative_path("dependencies/silicium");
				arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DSILICIUM_INCLUDE_DIR=") + to_os_string(silicium));
			}
			if (boost_root)
			{
				arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBOOST_ROOT=") + to_os_string(*boost_root));
				arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-DBoost_NO_SYSTEM_PATHS=ON"));
			}
			Si::absolute_path const modules = repository / Si::relative_path("modules");
			arguments.emplace_back(
				SILICIUM_SYSTEM_LITERAL("-DCDM_CONFIGURE_INCLUDE_DIRS=") +
				Si::to_os_string(application_source) + SILICIUM_SYSTEM_LITERAL(";") +
				Si::to_os_string(modules) + SILICIUM_SYSTEM_LITERAL(";") +
				Si::to_os_string(repository)
			);
			arguments.emplace_back(Si::to_os_string(source));
#ifdef _MSC_VER
			arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-G \"Visual Studio 12 2013\""));
#endif
			if (Si::run_process(Si::cmake_exe, arguments, build, output).get() != 0)
			{
				throw std::runtime_error("Could not CMake-configure the cdm configure executable");
			}
		}
		{
			std::vector<Si::os_string> arguments;
			arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("--build"));
			arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("."));
			if (Si::run_process(Si::cmake_exe, arguments, build, output).get() != 0)
			{
				throw std::runtime_error("Could not CMake --build the cdm configure executable");
			}
		}
		Si::absolute_path built_executable = build / Si::relative_path(
#ifdef _MSC_VER
			SILICIUM_SYSTEM_LITERAL("Debug/")
#endif
			SILICIUM_SYSTEM_LITERAL("configure")
#ifdef _WIN32
			SILICIUM_SYSTEM_LITERAL(".exe")
#endif
		);
		return built_executable;
	}

	void run_configure(
		Si::absolute_path const &configure_executable,
		Si::absolute_path const &module_permanent,
		Si::absolute_path const &application_source,
		Si::absolute_path const &application_build_dir,
		Si::Sink<char, Si::success>::interface &output)
	{
		Si::create_directories(module_permanent, Si::throw_);
		Si::create_directories(application_build_dir, Si::throw_);
		std::vector<Si::os_string> arguments;
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-m"));
		arguments.emplace_back(Si::to_os_string(module_permanent));
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-a"));
		arguments.emplace_back(Si::to_os_string(application_source));
		arguments.emplace_back(SILICIUM_SYSTEM_LITERAL("-b"));
		arguments.emplace_back(Si::to_os_string(application_build_dir));
		int const rc = Si::run_process(configure_executable, arguments, application_build_dir, output).get();
		if (rc != 0)
		{
			throw std::runtime_error("Could not configure the application: " + boost::lexical_cast<std::string>(rc));
		}
	}
}

namespace cdm
{
	void do_configure(
		Si::absolute_path const &temporary,
		Si::absolute_path const &module_permanent,
		Si::absolute_path const &application_source,
		Si::absolute_path const &application_build_dir,
		Si::optional<Si::absolute_path> const &boost_root,
		Si::Sink<char, Si::success>::interface &output
	)
	{
		Si::absolute_path const configure_executable = build_configure(application_source, temporary, boost_root, output);
		run_configure(configure_executable, module_permanent, application_source, application_build_dir, output);
	}
}
