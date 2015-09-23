#ifndef CDM_LOCATE_CACHE_HPP
#define CDM_LOCATE_CACHE_HPP

#include <silicium/file_operations.hpp>

namespace cdm
{
	inline Si::absolute_path locate_cache()
	{
		return Si::get_home() / Si::relative_path(".cdm_cache");
	}
}

#endif
