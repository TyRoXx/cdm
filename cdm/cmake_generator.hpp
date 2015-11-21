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
				                Si::append(arguments, "-G \"Visual Studio 12 2013\"");
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

	template <class StringSink>
	void generate_cmake_build_arguments(StringSink &&arguments, configuration const &target)
	{
		Si::append(arguments, "--build");
		Si::append(arguments, ".");
		Si::visit<void>(target.platform,
		                [&arguments, &target](windows_flavor const &)
		                {
			                Si::append(arguments, "--config");
			                Si::append(arguments, target.is_debug ? "Debug" : "Release");
			            },
		                [](gcc_version)
		                {
			            });
	}

	inline ventura::relative_path make_default_path_of_executable(ventura::path_segment const &name,
	                                                              configuration const &target)
	{
		return Si::visit<ventura::relative_path>(target.platform,
		                                         [&target, &name](windows_flavor const &)
		                                         {
			                                         return ventura::relative_path(target.is_debug ? "Debug"
			                                                                                       : "Release") /
			                                                (name + *ventura::path_segment::create(".exe"));
			                                     },
		                                         [&name](gcc_version)
		                                         {
			                                         return ventura::relative_path(name);
			                                     });
	}

	inline ventura::relative_path make_default_path_of_static_library(ventura::path_segment const &name,
	                                                                  configuration const &target)
	{
		return Si::visit<ventura::relative_path>(
		    target.platform,
		    [&target, &name](windows_flavor const &)
		    {
			    return ventura::relative_path(target.is_debug ? "Debug" : "Release") /
			           (name + *ventura::path_segment::create(".lib"));
			},
		    [&name](gcc_version)
		    {
			    return ventura::relative_path(*ventura::path_segment::create("lib") + name +
			                                  *ventura::path_segment::create(".a"));
			});
	}
}

#endif
