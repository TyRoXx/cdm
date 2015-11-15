#ifndef CDM_CMAKE_GENERATOR_HPP
#define CDM_CMAKE_GENERATOR_HPP

#include <silicium/sink/append.hpp>
#include <silicium/c_string.hpp>

namespace cdm
{
	template <class StringSink>
	void generate_default_cmake_generator_arguments(StringSink &&arguments)
	{
#ifdef _MSC_VER
		Si::append(arguments,
#if _MSC_VER == 1900
		           "-G \"Visual Studio 14 2015\""
#elif _MSC_VER == 1800
		           "-G \"Visual Studio 12 2013\""
#else
#error unsupported version of Visual Studio
#endif
		           );
#else
		Si::ignore_unused_variable_warning(arguments);
#endif
	}
}

#endif
