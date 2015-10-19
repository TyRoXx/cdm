#ifndef CDM_LOCATE_CACHE_HPP
#define CDM_LOCATE_CACHE_HPP

#include <ventura/file_operations.hpp>

namespace cdm
{
	inline ventura::absolute_path locate_cache()
	{
		return ventura::get_home() / ventura::relative_path(".cdm_cache");
	}
}

#endif
