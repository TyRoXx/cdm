#ifndef CDM_OPERATING_SYSTEM_HPP
#define CDM_OPERATING_SYSTEM_HPP

#include <silicium/variant.hpp>
#include <silicium/environment_variables.hpp>

namespace cdm
{
	struct ubuntu
	{
		bool is_travis_ci;

		explicit ubuntu(bool is_travis_ci)
		    : is_travis_ci(is_travis_ci)
		{
		}
	};

	struct windows
	{
		bool is_appveyor;

		explicit windows(bool is_appveyor)
		    : is_appveyor(is_appveyor)
		{
		}
	};

	typedef Si::variant<ubuntu, windows> operating_system;

	inline operating_system detect_this_binary_operating_system(bool is_travis_ci, bool is_appveyor)
	{
#ifdef _WIN32
		Si::ignore_unused_variable_warning(is_travis_ci);
		return windows(is_appveyor);
#else
		Si::ignore_unused_variable_warning(is_appveyor);
		return ubuntu(is_travis_ci);
#endif
	}
}

#endif
