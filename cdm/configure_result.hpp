#ifndef CDM_CONFIGURE_RESULT_HPP
#define CDM_CONFIGURE_RESULT_HPP

#include <silicium/absolute_path.hpp>

namespace cdm
{
	struct configure_result
	{
		std::vector<Si::absolute_path> shared_library_directories;
	};
}

#endif
