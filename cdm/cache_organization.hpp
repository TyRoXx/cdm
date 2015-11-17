#ifndef CDM_CACHE_ORGANIZATION_HPP
#define CDM_CACHE_ORGANIZATION_HPP

#include <ventura/relative_path.hpp>
#include <silicium/variant.hpp>
#include <silicium/memory_range.hpp>
#include <silicium/sink/iterator_sink.hpp>
#include <boost/lexical_cast.hpp>
#if BOOST_VERSION >= 105600
#include <boost/predef.h>
#endif

namespace cdm
{
	enum class gcc_version
	{
		v4_6,
		v4_7,
		v4_8,
		v4_9,
		v5_0
	};

	enum class windows_flavor
	{
		visual_studio_2013,
		visual_studio_2015
	};

	enum class cpu_architecture
	{
		amd64,
		x86
	};

	typedef Si::variant<windows_flavor, gcc_version> platform_flavor;

	struct configuration
	{
		cpu_architecture cpu;
		platform_flavor platform;
		bool is_debug;

		configuration()
		    : cpu(cpu_architecture::amd64)
		    , platform(windows_flavor::visual_studio_2013)
		    , is_debug(false)
		{
		}

		configuration(cpu_architecture cpu, platform_flavor platform, bool is_debug)
		    : cpu(cpu)
		    , platform(platform)
		    , is_debug(is_debug)
		{
		}
	};

	inline configuration approximate_configuration_of_this_binary()
	{
		return configuration(
#if BOOST_VERSION >= 105600
#if BOOST_ARCH_X86_32
		    cpu_architecture::x86
#elif BOOST_ARCH_X86_64
		    cpu_architecture::amd64
#else
#error unsupported CPU
#endif

#else
		    ((sizeof(void *) == 8) ? cpu_architecture::amd64 : cpu_architecture::x86)
#endif
		    ,
#ifdef _MSC_VER
#if _MSC_VER == 1900
		    windows_flavor::visual_studio_2015
#elif _MSC_VER == 1800
		    windows_flavor::visual_studio_2013
#else
#error unsupported version of Visual Studio
#endif
#elif defined(__linux__)
		    linux_flavor(__GLIBCXX__)
#else
#error unsupported compiler or operating system
#endif
		    ,
#ifdef NDEBUG
		    false
#else
		    true
#endif
		    );
	}

	char const configuration_element_separator = '&';

	template <class CharSink>
	inline void format_gcc(gcc_version gcc, CharSink &&output)
	{
		Si::append(output, "gcc-");
		switch (gcc)
		{
		case gcc_version::v4_6:
			Si::append(output, "4.6");
			break;
		case gcc_version::v4_7:
			Si::append(output, "4.7");
			break;
		case gcc_version::v4_8:
			Si::append(output, "4.8");
			break;
		case gcc_version::v4_9:
			Si::append(output, "4.9");
			break;
		case gcc_version::v5_0:
			Si::append(output, "5.0");
			break;
		}
	}

	inline Si::memory_range format_windows(windows_flavor windows)
	{
		switch (windows)
		{
		case windows_flavor::visual_studio_2013:
			return Si::make_c_str_range("vs2013");

		case windows_flavor::visual_studio_2015:
			return Si::make_c_str_range("vs2015");
		}
		SILICIUM_UNREACHABLE();
	}

	template <class CharSink>
	void format_configuration(configuration const &config, CharSink &&output)
	{
		switch (config.cpu)
		{
		case cpu_architecture::amd64:
			Si::append(output, "amd64");
			break;
		case cpu_architecture::x86:
			Si::append(output, "x86");
			break;
		}
		Si::append(output, configuration_element_separator);
		Si::visit<void>(config.platform,
		                [&output](gcc_version gcc)
		                {
			                format_gcc(gcc, output);
			            },
		                [&output](windows_flavor windows)
		                {
			                Si::append(output, format_windows(windows));
			            });
		Si::append(output, configuration_element_separator);
		Si::append(output, config.is_debug ? "debug" : "release");
	}

	inline ventura::relative_path get_configuration_in_cache(configuration const &config)
	{
		ventura::path::underlying_type path;
		auto path_sink = Si::make_container_sink(path);
		format_configuration(config, path_sink);
		return ventura::relative_path(std::move(path));
	}
}

#endif
