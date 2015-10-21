#ifndef CDM_RAPIDXML_HPP
#define CDM_RAPIDXML_HPP

#include <ventura/file_operations.hpp>

namespace cdm
{
	struct rapidxml_paths
	{
		ventura::absolute_path include;
	};

	inline rapidxml_paths install_rapidxml(ventura::absolute_path const &original_source,
	                                       ventura::absolute_path const &temporarily_writable,
	                                       ventura::absolute_path const &install_root,
	                                       Si::Sink<char, Si::success>::interface &output)
	{
		ventura::absolute_path const in_cache = install_root / ventura::relative_path("rapidxml");
		if (!ventura::file_exists(in_cache, Si::throw_))
		{
			ventura::absolute_path const construction = temporarily_writable / ventura::relative_path("rapidxml");
			ventura::copy_recursively(original_source, construction, &output, Si::throw_);
			ventura::rename(construction, in_cache, Si::throw_);
		}
		rapidxml_paths result;
		result.include = in_cache;
		return result;
	}
}

#endif
