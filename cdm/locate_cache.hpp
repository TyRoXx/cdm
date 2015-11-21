#ifndef CDM_LOCATE_CACHE_HPP
#define CDM_LOCATE_CACHE_HPP

#include <ventura/file_operations.hpp>
#include <cdm/cache_organization.hpp>

namespace cdm
{
	inline ventura::absolute_path locate_cache(configuration const &config)
	{
		return ventura::get_home() / ventura::relative_path(".cdm_cache") / get_configuration_in_cache(config);
	}
}

#endif
