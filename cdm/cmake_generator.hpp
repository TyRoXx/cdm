#ifndef CDM_CMAKE_GENERATOR_HPP
#define CDM_CMAKE_GENERATOR_HPP

#include <silicium/sink/append.hpp>
#include <silicium/c_string.hpp>
#include <cdm/cache_organization.hpp>

namespace cdm
{
	template <class StringSink>
	void generate_default_cmake_generator_arguments(StringSink &&arguments, configuration const &target)
	{
		Si::visit<void>(target.platform,
		                [&arguments](windows_flavor windows)
		                {
			                switch (windows)
			                {
			                case windows_flavor::visual_studio_2013:
				                Si::append(arguments, "-G \"Visual Studio 14 2013\"");
				                break;

			                case windows_flavor::visual_studio_2015:
				                Si::append(arguments, "-G \"Visual Studio 14 2015\"");
				                break;
			                }
			            },
		                [&arguments](gcc_version gcc)
		                {
			                switch (gcc)
			                {
			                case gcc_version::v4_6:
				                Si::append(arguments, "-DCMAKE_C_COMPILER=/usr/bin/gcc-4.6");
				                Si::append(arguments, "-DCMAKE_CXX_COMPILER=/usr/bin/g++-4.6");
				                break;

			                case gcc_version::v4_7:
				                Si::append(arguments, "-DCMAKE_C_COMPILER=/usr/bin/gcc-4.7");
				                Si::append(arguments, "-DCMAKE_CXX_COMPILER=/usr/bin/g++-4.7");
				                break;

			                case gcc_version::v4_8:
				                Si::append(arguments, "-DCMAKE_C_COMPILER=/usr/bin/gcc-4.8");
				                Si::append(arguments, "-DCMAKE_CXX_COMPILER=/usr/bin/g++-4.8");
				                break;

			                case gcc_version::v4_9:
				                Si::append(arguments, "-DCMAKE_C_COMPILER=/usr/bin/gcc-4.9");
				                Si::append(arguments, "-DCMAKE_CXX_COMPILER=/usr/bin/g++-4.9");
				                break;

			                case gcc_version::v5:
				                Si::append(arguments, "-DCMAKE_C_COMPILER=/usr/bin/gcc-5");
				                Si::append(arguments, "-DCMAKE_CXX_COMPILER=/usr/bin/g++-5");
				                break;
			                }
			            });
	}
}

#endif
