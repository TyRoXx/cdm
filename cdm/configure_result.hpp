#ifndef CDM_CONFIGURE_RESULT_HPP
#define CDM_CONFIGURE_RESULT_HPP

#include <ventura/absolute_path.hpp>

namespace cdm
{
	struct configure_result
	{
		std::vector<ventura::absolute_path> shared_library_directories;
	};
}

#endif
